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

#include <ql/indexes/indexmanager.hpp>
#include <qle/cashflows/fxlinkedcashflow.hpp>

namespace QuantExt {

FXLinked::FXLinked(const std::vector<Date>& fxFixingDates, Real foreignAmount, boost::shared_ptr<FxIndex> fxIndex)
    : fxFixingDates_(fxFixingDates), foreignAmount_(foreignAmount), fxIndex_(fxIndex) {}

FXLinked::FXLinked(const Date& fxFixingDate, Real foreignAmount, boost::shared_ptr<FxIndex> fxIndex)
    : fxFixingDates_(std::vector<Date>(1, fxFixingDate)), foreignAmount_(foreignAmount), fxIndex_(fxIndex) {}

Real FXLinked::fxRate() const {
    Real fx = 0;
    for (auto d: fxFixingDates_)
        fx += fxIndex_->fixing(d);
    fx /= fxFixingDates_.size();
    return fx;
}
  
FXLinkedCashFlow::FXLinkedCashFlow(const Date& cashFlowDate, const std::vector<Date>& fxFixingDates, Real foreignAmount,
                                   boost::shared_ptr<FxIndex> fxIndex)
    : FXLinked(fxFixingDates, foreignAmount, fxIndex), cashFlowDate_(cashFlowDate) {
    registerWith(FXLinked::fxIndex());
}

FXLinkedCashFlow::FXLinkedCashFlow(const Date& cashFlowDate, Date fxFixingDate, Real foreignAmount,
                                   boost::shared_ptr<FxIndex> fxIndex)
    : FXLinked(fxFixingDate, foreignAmount, fxIndex), cashFlowDate_(cashFlowDate) {
    registerWith(FXLinked::fxIndex());
}

boost::shared_ptr<FXLinked> FXLinkedCashFlow::clone(boost::shared_ptr<FxIndex> fxIndex) {
    return boost::make_shared<FXLinkedCashFlow>(date(), fxFixingDates(), foreignAmount(), fxIndex);
}

} // namespace QuantExt
