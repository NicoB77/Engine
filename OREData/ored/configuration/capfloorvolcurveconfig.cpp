/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <boost/algorithm/string.hpp>
#include <ored/configuration/capfloorvolcurveconfig.hpp>
#include <ored/utilities/indexparser.hpp>
#include <ored/utilities/parsers.hpp>
#include <ored/utilities/to_string.hpp>

#include <ql/errors.hpp>

namespace ore {
namespace data {

std::ostream& operator<<(std::ostream& out, CapFloorVolatilityCurveConfig::VolatilityType t) {
    switch (t) {
    case CapFloorVolatilityCurveConfig::VolatilityType::Lognormal:
        return out << "RATE_LNVOL";
    case CapFloorVolatilityCurveConfig::VolatilityType::Normal:
        return out << "RATE_NVOL";
    case CapFloorVolatilityCurveConfig::VolatilityType::ShiftedLognormal:
        return out << "RATE_SLNVOL";
    default:
        QL_FAIL("unknown VolatilityType(" << Integer(t) << ")");
    }
}

const string CapFloorVolatilityCurveConfig::defaultInterpolationMethod = "BicubicSpline";

CapFloorVolatilityCurveConfig::CapFloorVolatilityCurveConfig(
    const string& curveID, const string& curveDescription, const VolatilityType& volatilityType, const bool extrapolate,
    const bool flatExtrapolation, bool inlcudeAtm, const vector<std::string>& tenors, const vector<std::string>& strikes,
    const DayCounter& dayCounter, Natural settleDays, const Calendar& calendar,
    const BusinessDayConvention& businessDayConvention, const string& iborIndex, const string& discountCurve,
    const string& interpolationMethod)
    : CurveConfig(curveID, curveDescription), volatilityType_(volatilityType), extrapolate_(extrapolate),
      flatExtrapolation_(flatExtrapolation), includeAtm_(inlcudeAtm), tenors_(tenors), strikes_(strikes),
      dayCounter_(dayCounter), settleDays_(settleDays), calendar_(calendar),
      businessDayConvention_(businessDayConvention), iborIndex_(internalIndexName(iborIndex)), discountCurve_(discountCurve),
      interpolationMethod_(interpolationMethod) {}

const vector<string>& CapFloorVolatilityCurveConfig::quotes() {
    if (quotes_.size() == 0) {
        string tenor;
        boost::shared_ptr<IborIndex> index = parseIborIndex(iborIndex_, tenor);
        Currency ccy = index->currency();

        std::stringstream ssBase;
        ssBase << "CAPFLOOR/" << volatilityType_ << "/" << ccy.code() << "/";
        string base = ssBase.str();

        // TODO: how to tell if atmFlag or relative flag should be true
        for (auto t : tenors_) {
            for (auto s : strikes_) {
                quotes_.push_back(base + t + "/" + to_string(tenor) + "/0/0/" + s);
            }
        }

        if (volatilityType_ == VolatilityType::ShiftedLognormal) {
            for (auto t : tenors_) {
                std::stringstream ss;
                quotes_.push_back("CAPFLOOR/SHIFT/" + ccy.code() + "/" + t);
            }
        }
    }
    return quotes_;
}

void CapFloorVolatilityCurveConfig::fromXML(XMLNode* node) {
    XMLUtils::checkNode(node, "CapFloorVolatility");

    curveID_ = XMLUtils::getChildValue(node, "CurveId", true);
    curveDescription_ = XMLUtils::getChildValue(node, "CurveDescription", true);

    // We are requiring explicit strikes so there should be at least one strike
    strikes_ = XMLUtils::getChildrenValuesAsStrings(node, "Strikes", true);
    QL_REQUIRE(!strikes_.empty(), "Strikes node should not be empty");

    // Get the volatility type
    string volType = XMLUtils::getChildValue(node, "VolatilityType", true);
    if (volType == "Normal") {
        volatilityType_ = VolatilityType::Normal;
    } else if (volType == "Lognormal") {
        volatilityType_ = VolatilityType::Lognormal;
    } else if (volType == "ShiftedLognormal") {
        volatilityType_ = VolatilityType::ShiftedLognormal;
    } else {
        QL_FAIL("Volatility type, " << volType << ", not recognized");
    }
    includeAtm_ = XMLUtils::getChildValueAsBool(node, "IncludeAtm", true);

    string extr = XMLUtils::getChildValue(node, "Extrapolation", true);
    extrapolate_ = true;
    flatExtrapolation_ = true;
    if (extr == "Linear") {
        flatExtrapolation_ = false;
    } else if (extr == "Flat") {
        flatExtrapolation_ = true;
    } else if (extr == "None") {
        extrapolate_ = false;
    } else {
        QL_FAIL("Extrapolation " << extr << " not recognized");
    }

    tenors_ = XMLUtils::getChildrenValuesAsStrings(node, "Tenors", true);
    calendar_ = parseCalendar(XMLUtils::getChildValue(node, "Calendar", true));
    dayCounter_ = parseDayCounter(XMLUtils::getChildValue(node, "DayCounter", true));
    businessDayConvention_ = parseBusinessDayConvention(XMLUtils::getChildValue(node, "BusinessDayConvention", true));

    iborIndex_ = internalIndexName(XMLUtils::getChildValue(node, "IborIndex", true));
    discountCurve_ = XMLUtils::getChildValue(node, "DiscountCurve", true);
    interpolationMethod_ = XMLUtils::getChildValue(node, "InterpolationMethod", false);
    if (interpolationMethod_ == "")
        interpolationMethod_ = defaultInterpolationMethod;
}

XMLNode* CapFloorVolatilityCurveConfig::toXML(XMLDocument& doc) {
    XMLNode* node = doc.allocNode("CapFloorVolatility");

    XMLUtils::addChild(doc, node, "CurveId", curveID_);
    XMLUtils::addChild(doc, node, "CurveDescription", curveDescription_);

    if (volatilityType_ == VolatilityType::Normal) {
        XMLUtils::addChild(doc, node, "VolatilityType", "Normal");
    } else if (volatilityType_ == VolatilityType::Lognormal) {
        XMLUtils::addChild(doc, node, "VolatilityType", "Lognormal");
    } else if (volatilityType_ == VolatilityType::ShiftedLognormal) {
        XMLUtils::addChild(doc, node, "VolatilityType", "ShiftedLognormal");
    } else {
        QL_FAIL("Unknown VolatilityType in CapFloorVolatilityCurveConfig::toXML()");
    }

    string extr_str = flatExtrapolation_ ? "Flat" : "Linear";
    if (!extrapolate_)
        extr_str = "None";
    XMLUtils::addChild(doc, node, "Extrapolation", extr_str);
    XMLUtils::addChild(doc, node, "InterpolationMethod", interpolationMethod_);

    XMLUtils::addChild(doc, node, "IncludeAtm", includeAtm_);
    XMLUtils::addChild(doc, node, "DayCounter", to_string(dayCounter_));
    XMLUtils::addChild(doc, node, "Calendar", to_string(calendar_));
    XMLUtils::addChild(doc, node, "BusinessDayConvention", to_string(businessDayConvention_));
    XMLUtils::addGenericChildAsList(doc, node, "Tenors", tenors_);
    XMLUtils::addGenericChildAsList(doc, node, "Strikes", strikes_);
    XMLUtils::addChild(doc, node, "IborIndex", iborIndex_);
    XMLUtils::addChild(doc, node, "DiscountCurve", discountCurve_);

    return node;
}
} // namespace data
} // namespace ore
