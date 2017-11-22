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

#include <test/testmarket.hpp>
#include <ql/termstructures/inflation/inflationhelpers.hpp>
#include <ql/termstructures/inflation/piecewisezeroinflationcurve.hpp>
#include <ql/termstructures/inflation/piecewiseyoyinflationcurve.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/indexes/inflation/ukrpi.hpp>
#include <qle/indexes/inflationindexwrapper.hpp>
#include <boost/make_shared.hpp>
namespace testsuite {

TestMarket::TestMarket(Date asof) {
    asof_ = asof;

    // add conventions
    boost::shared_ptr<ore::data::Convention> swapIndexEURConv(
        new ore::data::SwapIndexConvention("EUR-CMS-2Y", "EUR-6M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexEURLongConv(
        new ore::data::SwapIndexConvention("EUR-CMS-30Y", "EUR-6M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexUSDConv(
        new ore::data::SwapIndexConvention("USD-CMS-2Y", "USD-3M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexUSDLongConv(
        new ore::data::SwapIndexConvention("USD-CMS-30Y", "USD-3M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexGBPConv(
        new ore::data::SwapIndexConvention("GBP-CMS-2Y", "GBP-3M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexGBPLongConv(
        new ore::data::SwapIndexConvention("GBP-CMS-30Y", "GBP-6M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexCHFConv(
        new ore::data::SwapIndexConvention("CHF-CMS-2Y", "CHF-3M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexCHFLongConv(
        new ore::data::SwapIndexConvention("CHF-CMS-30Y", "CHF-6M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexJPYConv(
        new ore::data::SwapIndexConvention("JPY-CMS-2Y", "JPY-LIBOR-6M-SWAP-CONVENTIONS"));
    boost::shared_ptr<ore::data::Convention> swapIndexJPYLongConv(
        new ore::data::SwapIndexConvention("JPY-CMS-30Y", "JPY-LIBOR-6M-SWAP-CONVENTIONS"));

    conventions_.add(swapIndexEURConv);
    conventions_.add(swapIndexEURLongConv);
    conventions_.add(swapIndexUSDConv);
    conventions_.add(swapIndexUSDLongConv);
    conventions_.add(swapIndexGBPConv);
    conventions_.add(swapIndexGBPLongConv);
    conventions_.add(swapIndexCHFConv);
    conventions_.add(swapIndexCHFLongConv);
    conventions_.add(swapIndexJPYConv);
    conventions_.add(swapIndexJPYLongConv);

    boost::shared_ptr<ore::data::Convention> swapEURConv(new ore::data::IRSwapConvention(
        "EUR-6M-SWAP-CONVENTIONS", "TARGET", "Annual", "MF", "30/360", "EUR-EURIBOR-6M"));
    boost::shared_ptr<ore::data::Convention> swapUSDConv(new ore::data::IRSwapConvention(
        "USD-3M-SWAP-CONVENTIONS", "US", "Semiannual", "MF", "30/360", "USD-LIBOR-3M"));
    boost::shared_ptr<ore::data::Convention> swapGBPConv(new ore::data::IRSwapConvention(
        "GBP-3M-SWAP-CONVENTIONS", "UK", "Semiannual", "MF", "A365", "GBP-LIBOR-3M"));
    boost::shared_ptr<ore::data::Convention> swapGBPLongConv(new ore::data::IRSwapConvention(
        "GBP-6M-SWAP-CONVENTIONS", "UK", "Semiannual", "MF", "A365", "GBP-LIBOR-6M"));
    boost::shared_ptr<ore::data::Convention> swapCHFConv(new ore::data::IRSwapConvention(
        "CHF-3M-SWAP-CONVENTIONS", "ZUB", "Annual", "MF", "30/360", "CHF-LIBOR-3M"));
    boost::shared_ptr<ore::data::Convention> swapCHFLongConv(new ore::data::IRSwapConvention(
        "CHF-6M-SWAP-CONVENTIONS", "ZUB", "Annual", "MF", "30/360", "CHF-LIBOR-6M"));
    boost::shared_ptr<ore::data::Convention> swapJPYConv(new ore::data::IRSwapConvention(
        "JPY-LIBOR-6M-SWAP-CONVENTIONS", "JP", "Semiannual", "MF", "A365", "JPY-LIBOR-6M"));

    conventions_.add(swapEURConv);
    conventions_.add(swapUSDConv);
    conventions_.add(swapGBPConv);
    conventions_.add(swapGBPLongConv);
    conventions_.add(swapCHFConv);
    conventions_.add(swapCHFLongConv);
    conventions_.add(swapJPYConv);

    // build discount
    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount, "EUR")] = flatRateYts(0.02);
    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount, "USD")] = flatRateYts(0.03);
    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount, "GBP")] = flatRateYts(0.04);
    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount, "CHF")] = flatRateYts(0.01);
    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount, "JPY")] = flatRateYts(0.005);

    // build ibor indices
    vector<pair<string, Real>> indexData = {
        { "EUR-EONIA", 0.01 },{ "EUR-EURIBOR-6M", 0.02 },{ "USD-FedFunds", 0.01 },{ "USD-LIBOR-3M", 0.03 },
        { "USD-LIBOR-6M", 0.05 },{ "GBP-SONIA", 0.01 },{ "GBP-LIBOR-3M", 0.03 },{ "GBP-LIBOR-6M", 0.04 },
        { "CHF-LIBOR-3M", 0.01 },{ "CHF-LIBOR-6M", 0.02 },{ "JPY-LIBOR-6M", 0.01 } };
    for (auto id : indexData) {
        Handle<IborIndex> h(parseIborIndex(id.first, flatRateYts(id.second)));
        iborIndices_[make_pair(Market::defaultConfiguration, id.first)] = h;

        // set up dummy fixings for the past 400 days
        for (Date d = asof - 400; d < asof; d++) {
            if (h->isValidFixingDate(d))
                h->addFixing(d, 0.01);
        }
    }

    // swap index
    addSwapIndex("EUR-CMS-2Y", "EUR-EONIA", Market::defaultConfiguration);
    addSwapIndex("EUR-CMS-30Y", "EUR-EONIA", Market::defaultConfiguration);
    addSwapIndex("USD-CMS-2Y", "USD-FedFunds", Market::defaultConfiguration);
    addSwapIndex("USD-CMS-30Y", "USD-FedFunds", Market::defaultConfiguration);
    addSwapIndex("GBP-CMS-2Y", "GBP-SONIA", Market::defaultConfiguration);
    addSwapIndex("GBP-CMS-30Y", "GBP-SONIA", Market::defaultConfiguration);
    addSwapIndex("CHF-CMS-2Y", "CHF-LIBOR-6M", Market::defaultConfiguration);
    addSwapIndex("CHF-CMS-30Y", "CHF-LIBOR-6M", Market::defaultConfiguration);
    addSwapIndex("JPY-CMS-2Y", "JPY-LIBOR-6M", Market::defaultConfiguration);
    addSwapIndex("JPY-CMS-30Y", "JPY-LIBOR-6M", Market::defaultConfiguration);

    // add fx rates
    fxSpots_[Market::defaultConfiguration].addQuote("EURUSD", Handle<Quote>(boost::make_shared<SimpleQuote>(1.2)));
    fxSpots_[Market::defaultConfiguration].addQuote("EURGBP", Handle<Quote>(boost::make_shared<SimpleQuote>(0.8)));
    fxSpots_[Market::defaultConfiguration].addQuote("EURCHF", Handle<Quote>(boost::make_shared<SimpleQuote>(1.0)));
    fxSpots_[Market::defaultConfiguration].addQuote("EURJPY",
        Handle<Quote>(boost::make_shared<SimpleQuote>(128.0)));

    // build fx vols
    fxVols_[make_pair(Market::defaultConfiguration, "EURUSD")] = flatRateFxv(0.12);
    fxVols_[make_pair(Market::defaultConfiguration, "EURGBP")] = flatRateFxv(0.15);
    fxVols_[make_pair(Market::defaultConfiguration, "EURCHF")] = flatRateFxv(0.15);
    fxVols_[make_pair(Market::defaultConfiguration, "EURJPY")] = flatRateFxv(0.15);
    fxVols_[make_pair(Market::defaultConfiguration, "GBPCHF")] = flatRateFxv(0.15);

    // Add Equity Spots
    equitySpots_[make_pair(Market::defaultConfiguration, "SP5")] =
        Handle<Quote>(boost::make_shared<SimpleQuote>(2147.56));
    equitySpots_[make_pair(Market::defaultConfiguration, "Lufthansa")] =
        Handle<Quote>(boost::make_shared<SimpleQuote>(12.75));

    equityVols_[make_pair(Market::defaultConfiguration, "SP5")] = flatRateFxv(0.2514);
    equityVols_[make_pair(Market::defaultConfiguration, "Lufthansa")] = flatRateFxv(0.30);

    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::EquityDividend, "SP5")] = flatRateDiv(0.01);
    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::EquityDividend, "Lufthansa")] = flatRateDiv(0.0);

    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::EquityForecast, "SP5")] = flatRateYts(0.03);
    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::EquityForecast, "Lufthansa")] = flatRateYts(0.02);
    // build swaption vols
    swaptionCurves_[make_pair(Market::defaultConfiguration, "EUR")] = flatRateSvs(0.20);
    swaptionCurves_[make_pair(Market::defaultConfiguration, "USD")] = flatRateSvs(0.30);
    swaptionCurves_[make_pair(Market::defaultConfiguration, "GBP")] = flatRateSvs(0.25);
    swaptionCurves_[make_pair(Market::defaultConfiguration, "CHF")] = flatRateSvs(0.25);
    swaptionCurves_[make_pair(Market::defaultConfiguration, "JPY")] = flatRateSvs(0.25);
    swaptionIndexBases_[make_pair(Market::defaultConfiguration, "EUR")] =
        std::make_pair("EUR-CMS-2Y", "EUR-CMS-30Y");
    swaptionIndexBases_[make_pair(Market::defaultConfiguration, "USD")] =
        std::make_pair("USD-CMS-2Y", "USD-CMS-30Y");
    swaptionIndexBases_[make_pair(Market::defaultConfiguration, "GBP")] =
        std::make_pair("GBP-CMS-2Y", "GBP-CMS-30Y");
    swaptionIndexBases_[make_pair(Market::defaultConfiguration, "CHF")] =
        std::make_pair("CHF-CMS-2Y", "CHF-CMS-30Y");
    swaptionIndexBases_[make_pair(Market::defaultConfiguration, "JPY")] =
        std::make_pair("JPY-CMS-2Y", "JPY-CMS-30Y");

    // build cap/floor vol structures
    capFloorCurves_[make_pair(Market::defaultConfiguration, "EUR")] = flatRateCvs(0.0050, Normal);
    capFloorCurves_[make_pair(Market::defaultConfiguration, "USD")] = flatRateCvs(0.0060, Normal);
    capFloorCurves_[make_pair(Market::defaultConfiguration, "GBP")] = flatRateCvs(0.0055, Normal);
    capFloorCurves_[make_pair(Market::defaultConfiguration, "CHF")] = flatRateCvs(0.0045, Normal);
    capFloorCurves_[make_pair(Market::defaultConfiguration, "JPY")] = flatRateCvs(0.0040, Normal);

    // build default curves
    defaultCurves_[make_pair(Market::defaultConfiguration, "dc")] = flatRateDcs(0.1);
    defaultCurves_[make_pair(Market::defaultConfiguration, "dc2")] = flatRateDcs(0.2);
    defaultCurves_[make_pair(Market::defaultConfiguration, "BondIssuer1")] = flatRateDcs(0.0);

    recoveryRates_[make_pair(Market::defaultConfiguration, "dc")] =
        Handle<Quote>(boost::make_shared<SimpleQuote>(0.4));
    recoveryRates_[make_pair(Market::defaultConfiguration, "dc2")] =
        Handle<Quote>(boost::make_shared<SimpleQuote>(0.4));
    recoveryRates_[make_pair(Market::defaultConfiguration, "BondIssuer1")] =
        Handle<Quote>(boost::make_shared<SimpleQuote>(0.0));

    yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Yield, "BondCurve1")] = flatRateYts(0.05);

    securitySpreads_[make_pair(Market::defaultConfiguration, "Bond1")] =
        Handle<Quote>(boost::make_shared<SimpleQuote>(0.0));

    Handle<IborIndex> hGBP(ore::data::parseIborIndex(
        "GBP-LIBOR-6M", yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount, "GBP")]));
    iborIndices_[make_pair(Market::defaultConfiguration, "GBP-LIBOR-6M")] = hGBP;

    // build UKRPI fixing history
    Date cpiFixingEnd(1, asof_.month(), asof_.year());
    Date cpiFixingStart = cpiFixingEnd - Period(14, Months);
    Schedule fixingDatesUKRPI =
        MakeSchedule().from(cpiFixingStart).to(cpiFixingEnd).withTenor(1 * Months);
    Real fixingRatesUKRPI[] = { 258.5, 258.9, 258.6, 259.8, 259.6, 259.5,  259.8, 260.6,
        258.8, 260.0, 261.1, 261.4, 262.1, -264.3, -265.2 };

    // build UKRPI index
    boost::shared_ptr<ZeroInflationIndex> ii = parseZeroInflationIndex("UKRPI");
    boost::shared_ptr<YoYInflationIndex> yi =
        boost::make_shared<QuantExt::YoYInflationIndexWrapper>(ii, false);

    RelinkableHandle<ZeroInflationTermStructure> hcpi;
    bool interp = false;
    ii = boost::shared_ptr<UKRPI>(new UKRPI(interp, hcpi));
    for (Size i = 0; i < fixingDatesUKRPI.size(); i++) {
        // std::cout << i << ", " << fixingDatesUKRPI[i] << ", " << fixingRatesUKRPI[i] << std::endl;
        ii->addFixing(fixingDatesUKRPI[i], fixingRatesUKRPI[i], true);
    };

    vector<Date> datesZCII = { asof_,
        asof_ + 1 * Years,
        asof_ + 2 * Years,
        asof_ + 3 * Years,
        asof_ + 4 * Years,
        asof_ + 5 * Years,
        asof_ + 6 * Years,
        asof_ + 7 * Years,
        asof_ + 8 * Years,
        asof_ + 9 * Years,
        asof_ + 10 * Years,
        asof_ + 12 * Years,
        asof_ + 15 * Years,
        asof_ + 20 * Years };

    vector<Rate> ratesZCII = { 2.825, 2.9425, 2.975,  2.983, 3.0,  3.01,  3.008,
        3.009, 3.013,  3.0445, 3.044, 3.09, 3.109, 3.108 };

    zeroInflationIndices_[make_pair(Market::defaultConfiguration, "UKRPI")] = makeZeroInflationIndex("UKRPI", datesZCII, ratesZCII, ii, yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount, "GBP")]);
    yoyInflationIndices_[make_pair(Market::defaultConfiguration, "UKRPI")] = makeYoYInflationIndex("UKRPI", datesZCII, ratesZCII, yi, yieldCurves_[make_tuple(Market::defaultConfiguration, YieldCurveType::Discount,"GBP")]);
    
}

Handle<ZeroInflationIndex> TestMarket::makeZeroInflationIndex(string index, vector<Date> dates, vector<Rate> rates, 
                                                           boost::shared_ptr<ZeroInflationIndex> ii, Handle<YieldTermStructure> yts) {
    // build UKRPI index

    boost::shared_ptr<ZeroInflationTermStructure> cpiTS;
    // now build the helpers ...
    vector<boost::shared_ptr<BootstrapHelper<ZeroInflationTermStructure>>> instruments;
    for (Size i = 0; i < dates.size(); i++) {
        Handle<Quote> quote(boost::shared_ptr<Quote>(new SimpleQuote(rates[i] / 100.0)));
        boost::shared_ptr<BootstrapHelper<ZeroInflationTermStructure>> anInstrument(
            new ZeroCouponInflationSwapHelper(quote, Period(2, Months), dates[i], TARGET(),
                ModifiedFollowing, ActualActual(), ii));
        instruments.push_back(anInstrument);
    };
    // we can use historical or first ZCIIS for this
    // we know historical is WAY off market-implied, so use market implied flat.
    Rate baseZeroRate = rates[0] / 100.0;
    boost::shared_ptr<PiecewiseZeroInflationCurve<Linear>> pCPIts(new PiecewiseZeroInflationCurve<Linear>(
        asof_, TARGET(), ActualActual(), Period(2, Months), ii->frequency(), ii->interpolated(),
        baseZeroRate, yts, instruments));
    pCPIts->recalculate();
    cpiTS = boost::dynamic_pointer_cast<ZeroInflationTermStructure>(pCPIts);
    return Handle<ZeroInflationIndex>(
        parseZeroInflationIndex(index, false, Handle<ZeroInflationTermStructure>(cpiTS)));
}

Handle<YoYInflationIndex> TestMarket::makeYoYInflationIndex(string index, vector<Date> dates, vector<Rate> rates,
    boost::shared_ptr<YoYInflationIndex> ii, Handle<YieldTermStructure> yts) {
    // build UKRPI index

    boost::shared_ptr<YoYInflationTermStructure> yoyTS;
    // now build the helpers ...
    vector<boost::shared_ptr<BootstrapHelper<YoYInflationTermStructure>>> instruments;
    for (Size i = 0; i < dates.size(); i++) {
        Handle<Quote> quote(boost::shared_ptr<Quote>(new SimpleQuote(rates[i] / 100.0)));
        boost::shared_ptr<BootstrapHelper<YoYInflationTermStructure>> anInstrument(
            new  YearOnYearInflationSwapHelper(quote, Period(2, Months), dates[i], TARGET(),
                ModifiedFollowing, ActualActual(), ii));
        instruments.push_back(anInstrument);
    };
    // we can use historical or first ZCIIS for this
    // we know historical is WAY off market-implied, so use market implied flat.
    Rate baseZeroRate = rates[0] / 100.0;
    boost::shared_ptr<PiecewiseYoYInflationCurve<Linear>> pYoYts(new PiecewiseYoYInflationCurve<Linear>(
        asof_, TARGET(), ActualActual(), Period(2, Months), ii->frequency(), ii->interpolated(),
        baseZeroRate, yts, instruments));
    pYoYts->recalculate();
    yoyTS = boost::dynamic_pointer_cast<YoYInflationTermStructure>(pYoYts);
    return Handle<YoYInflationIndex>(boost::make_shared<QuantExt::YoYInflationIndexWrapper>(
        parseZeroInflationIndex(index, false), false, Handle<YoYInflationTermStructure>(pYoYts)));
}

boost::shared_ptr<ore::data::Conventions> TestConfigurationObjects::conv() {
    boost::shared_ptr<ore::data::Conventions> conventions(new ore::data::Conventions());

    conventions->add(boost::make_shared<ore::data::SwapIndexConvention>("EUR-CMS-2Y", "EUR-6M-SWAP-CONVENTIONS"));
    conventions->add(boost::make_shared<ore::data::SwapIndexConvention>("EUR-CMS-30Y", "EUR-6M-SWAP-CONVENTIONS"));

    // boost::shared_ptr<data::Convention> swapConv(
    //     new data::IRSwapConvention("EUR-6M-SWAP-CONVENTIONS", "TARGET", "Annual", "MF", "30/360", "EUR-EURIBOR-6M"));
    conventions->add(boost::make_shared<ore::data::IRSwapConvention>("EUR-6M-SWAP-CONVENTIONS", "TARGET", "A", "MF",
                                                                     "30/360", "EUR-EURIBOR-6M"));
    conventions->add(boost::make_shared<ore::data::IRSwapConvention>("USD-3M-SWAP-CONVENTIONS", "TARGET", "Q", "MF",
                                                                     "30/360", "USD-LIBOR-3M"));
    conventions->add(boost::make_shared<ore::data::IRSwapConvention>("USD-6M-SWAP-CONVENTIONS", "TARGET", "Q", "MF",
                                                                     "30/360", "USD-LIBOR-6M"));
    conventions->add(boost::make_shared<ore::data::IRSwapConvention>("GBP-6M-SWAP-CONVENTIONS", "TARGET", "A", "MF",
                                                                     "30/360", "GBP-LIBOR-6M"));
    conventions->add(boost::make_shared<ore::data::IRSwapConvention>("JPY-6M-SWAP-CONVENTIONS", "TARGET", "A", "MF",
                                                                     "30/360", "JPY-LIBOR-6M"));
    conventions->add(boost::make_shared<ore::data::IRSwapConvention>("CHF-6M-SWAP-CONVENTIONS", "TARGET", "A", "MF",
                                                                     "30/360", "CHF-LIBOR-6M"));

    conventions->add(boost::make_shared<ore::data::DepositConvention>("EUR-DEP-CONVENTIONS", "EUR-EURIBOR"));
    conventions->add(boost::make_shared<ore::data::DepositConvention>("USD-DEP-CONVENTIONS", "USD-LIBOR"));
    conventions->add(boost::make_shared<ore::data::DepositConvention>("GBP-DEP-CONVENTIONS", "GBP-LIBOR"));
    conventions->add(boost::make_shared<ore::data::DepositConvention>("JPY-DEP-CONVENTIONS", "JPY-LIBOR"));
    conventions->add(boost::make_shared<ore::data::DepositConvention>("CHF-DEP-CONVENTIONS", "CHF-LIBOR"));

    return conventions;
}

boost::shared_ptr<ore::analytics::ScenarioSimMarketParameters> TestConfigurationObjects::setupSimMarketData2() {
    boost::shared_ptr<ore::analytics::ScenarioSimMarketParameters> simMarketData(
        new ore::analytics::ScenarioSimMarketParameters());
    simMarketData->baseCcy() = "EUR";
    simMarketData->ccys() = {"EUR", "GBP"};
    simMarketData->yieldCurveNames() = {"BondCurve1"};
    simMarketData->setYieldCurveTenors("", {1 * Months, 6 * Months, 1 * Years, 2 * Years, 3 * Years, 4 * Years,
                                            5 * Years, 6 * Years, 7 * Years, 8 * Years, 9 * Years, 10 * Years,
                                            12 * Years, 15 * Years, 20 * Years, 25 * Years, 30 * Years});
    simMarketData->setYieldCurveDayCounters("", "ACT/ACT");
    simMarketData->indices() = {"EUR-EURIBOR-6M", "GBP-LIBOR-6M"};
    simMarketData->defaultNames() = {"BondIssuer1"};
    simMarketData->setDefaultTenors(
        "", {6 * Months, 1 * Years, 2 * Years, 3 * Years, 5 * Years, 7 * Years, 10 * Years, 15 * Years, 20 * Years});
    simMarketData->securities() = {"Bond1"};
    simMarketData->simulateSurvivalProbabilities() = true;
    simMarketData->setDefaultCurveDayCounters("", "ACT/ACT");
    simMarketData->setDefaultCurveCalendars("", "TARGET");
    simMarketData->interpolation() = "LogLinear";
    simMarketData->extrapolate() = true;

    simMarketData->swapVolTerms() = {1 * Years, 2 * Years, 3 * Years,  4 * Years,
                                     5 * Years, 7 * Years, 10 * Years, 20 * Years};
    simMarketData->swapVolExpiries() = {6 * Months, 1 * Years, 2 * Years,  3 * Years,
                                        5 * Years,  7 * Years, 10 * Years, 20 * Years};
    simMarketData->swapVolCcys() = {"EUR", "GBP"};
    simMarketData->swapVolDecayMode() = "ForwardVariance";
    simMarketData->simulateSwapVols() = true;
    simMarketData->setSwapVolDayCounters("", "ACT/ACT");

    simMarketData->fxVolExpiries() = {1 * Months, 3 * Months, 6 * Months, 2 * Years, 3 * Years, 4 * Years, 5 * Years};
    simMarketData->fxVolDecayMode() = "ConstantVariance";
    simMarketData->simulateFXVols() = true;
    simMarketData->fxVolCcyPairs() = {"EURGBP"};
    simMarketData->fxVolIsSurface() = true;
    simMarketData->fxVolMoneyness() = {0.1, 0.2, 0.3, 0.5, 1, 2, 3};

    simMarketData->fxCcyPairs() = {"EURGBP"};

    simMarketData->simulateCapFloorVols() = false;

    return simMarketData;
}

boost::shared_ptr<ore::analytics::ScenarioSimMarketParameters> TestConfigurationObjects::setupSimMarketData5() {
    boost::shared_ptr<ore::analytics::ScenarioSimMarketParameters> simMarketData(
        new ore::analytics::ScenarioSimMarketParameters());

    simMarketData->baseCcy() = "EUR";
    simMarketData->ccys() = {"EUR", "GBP", "USD", "CHF", "JPY"};
    simMarketData->setYieldCurveTenors("", {1 * Months, 6 * Months, 1 * Years, 2 * Years, 3 * Years, 4 * Years,
                                            5 * Years, 7 * Years, 10 * Years, 15 * Years, 20 * Years, 30 * Years});
    simMarketData->setYieldCurveDayCounters("", "ACT/ACT");
    simMarketData->indices() = {"EUR-EURIBOR-6M", "USD-LIBOR-3M", "USD-LIBOR-6M",
                                "GBP-LIBOR-6M",   "CHF-LIBOR-6M", "JPY-LIBOR-6M"};
    simMarketData->swapIndices() = {{"EUR-CMS-2Y", "EUR-EURIBOR-6M"}, {"EUR-CMS-30Y", "EUR-EURIBOR-6M"}};
    simMarketData->yieldCurveNames() = {"BondCurve1"};
    simMarketData->interpolation() = "LogLinear";
    simMarketData->extrapolate() = true;

    simMarketData->swapVolTerms() = {1 * Years, 2 * Years, 3 * Years, 5 * Years, 7 * Years, 10 * Years, 20 * Years};
    simMarketData->swapVolExpiries() = {6 * Months, 1 * Years, 2 * Years,  3 * Years,
                                        5 * Years,  7 * Years, 10 * Years, 20 * Years};
    simMarketData->swapVolCcys() = {"EUR", "GBP", "USD", "CHF", "JPY"};
    simMarketData->swapVolDecayMode() = "ForwardVariance";
    simMarketData->simulateSwapVols() = true; // false;
    simMarketData->setSwapVolDayCounters("", "ACT/ACT");

    simMarketData->fxVolExpiries() = {1 * Months, 3 * Months, 6 * Months, 2 * Years, 3 * Years, 4 * Years, 5 * Years};
    simMarketData->fxVolDecayMode() = "ConstantVariance";
    simMarketData->simulateFXVols() = true; // false;
    simMarketData->fxVolCcyPairs() = {"EURUSD", "EURGBP", "EURCHF", "EURJPY", "GBPCHF"};
    simMarketData->fxVolIsSurface() = true;
    simMarketData->fxVolMoneyness() = {0.1, 0.2, 0.3, 0.5, 1, 2, 3};

    simMarketData->fxCcyPairs() = { "EURUSD", "EURGBP", "EURCHF", "EURJPY" };

    simMarketData->simulateCapFloorVols() = true;
    simMarketData->capFloorVolDecayMode() = "ForwardVariance";
    simMarketData->capFloorVolCcys() = {"EUR", "USD"};
    simMarketData->setCapFloorVolDayCounters("", "ACT/ACT");
    simMarketData->setCapFloorVolExpiries(
        "", {6 * Months, 1 * Years, 2 * Years, 3 * Years, 5 * Years, 7 * Years, 10 * Years, 15 * Years, 20 * Years});
    simMarketData->capFloorVolStrikes() = {0.00, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06};

    simMarketData->defaultNames() = {"BondIssuer1"};
    simMarketData->setDefaultTenors(
        "", {6 * Months, 1 * Years, 2 * Years, 3 * Years, 5 * Years, 7 * Years, 10 * Years, 15 * Years, 20 * Years});
    simMarketData->simulateSurvivalProbabilities() = true;
    simMarketData->securities() = {"Bond1"};
    simMarketData->setDefaultCurveDayCounters("", "ACT/ACT");
    simMarketData->setDefaultCurveCalendars("", "TARGET");

    simMarketData->equityNames() = { "SP5", "Lufthansa" };
    simMarketData->setEquityDividendTenors("SP5", { 6 * Months, 1 * Years, 2 * Years });
    simMarketData->setEquityDividendTenors("Lufthansa", { 6 * Months, 1 * Years, 2 * Years });
    simMarketData->setEquityForecastTenors("SP5", {1 * Months, 6 * Months, 1 * Years, 2 * Years, 3 * Years, 4 * Years,
                                            5 * Years, 7 * Years, 10 * Years, 15 * Years, 20 * Years, 30 * Years });
    simMarketData->setEquityForecastTenors("Lufthansa", {1 * Months, 6 * Months, 1 * Years, 2 * Years, 3 * Years, 
                                        4 * Years, 5 * Years, 7 * Years, 10 * Years, 15 * Years, 20 * Years, 30 * Years});

    simMarketData->simulateEquityVols() = true;
    simMarketData->equityVolDecayMode() = "ForwardVariance";
    simMarketData->equityVolNames() = { "SP5", "Lufthansa" };
    simMarketData->equityVolExpiries() = { 6 * Months, 1 * Years, 2 * Years,  3 * Years,
                                           5 * Years,  7 * Years, 10 * Years, 20 * Years };
    simMarketData->equityVolIsSurface() = false;
    simMarketData->simulateEquityVolATMOnly() = true;
    simMarketData->equityVolMoneyness() = {1};

    simMarketData->zeroInflationIndices() = { "UKRPI" };
    simMarketData->setZeroInflationTenors("UKRPI", { 1 * Years,  2 * Years,  3 * Years, 5 * Years,
        7 * Years,  10 * Years, 15 * Years, 20 * Years });
    simMarketData->setZeroInflationDayCounters("", "ACT/ACT");
    simMarketData->yoyInflationIndices() = { "UKRPI" };
    simMarketData->setYoyInflationTenors("UKRPI", { 1 * Years,  2 * Years,  3 * Years, 5 * Years,
        7 * Years,  10 * Years, 15 * Years, 20 * Years });
    simMarketData->setYoyInflationDayCounters("", "ACT/ACT");

    return simMarketData;
}

boost::shared_ptr<ore::analytics::SensitivityScenarioData> TestConfigurationObjects::setupSensitivityScenarioData2() {
    boost::shared_ptr<ore::analytics::SensitivityScenarioData> sensiData =
        boost::make_shared<ore::analytics::SensitivityScenarioData>();

    ore::analytics::SensitivityScenarioData::CurveShiftData cvsData;
    cvsData.shiftTenors = {1 * Years, 2 * Years,  3 * Years,  5 * Years,
                           7 * Years, 10 * Years, 15 * Years, 20 * Years}; // multiple tenors: triangular shifts
    cvsData.shiftType = "Absolute";
    cvsData.shiftSize = 0.0001;

    ore::analytics::SensitivityScenarioData::SpotShiftData fxsData;
    fxsData.shiftType = "Relative";
    fxsData.shiftSize = 0.01;

    ore::analytics::SensitivityScenarioData::VolShiftData fxvsData;
    fxvsData.shiftType = "Relative";
    fxvsData.shiftSize = 1.0;
    fxvsData.shiftExpiries = {2 * Years, 5 * Years};

    ore::analytics::SensitivityScenarioData::CapFloorVolShiftData cfvsData;
    cfvsData.shiftType = "Absolute";
    cfvsData.shiftSize = 0.0001;
    cfvsData.shiftExpiries = {1 * Years, 2 * Years, 3 * Years, 5 * Years, 10 * Years};
    cfvsData.shiftStrikes = {0.05};

    ore::analytics::SensitivityScenarioData::SwaptionVolShiftData swvsData;
    swvsData.shiftType = "Relative";
    swvsData.shiftSize = 0.01;
    swvsData.shiftExpiries = {3 * Years, 5 * Years, 10 * Years};
    swvsData.shiftTerms = {2 * Years, 5 * Years, 10 * Years};

    sensiData->discountCurrencies() = {"EUR", "GBP"};
    sensiData->discountCurveShiftData()["EUR"] = cvsData;

    sensiData->discountCurveShiftData()["GBP"] = cvsData;

    sensiData->indexNames() = {"EUR-EURIBOR-6M", "GBP-LIBOR-6M"};
    sensiData->indexCurveShiftData()["EUR-EURIBOR-6M"] = cvsData;
    sensiData->indexCurveShiftData()["GBP-LIBOR-6M"] = cvsData;

    sensiData->yieldCurveNames() = {"BondCurve1"};
    sensiData->yieldCurveShiftData()["BondCurve1"] = cvsData;

    sensiData->fxCcyPairs() = {"EURGBP"};
    sensiData->fxShiftData()["EURGBP"] = fxsData;

    sensiData->fxVolCcyPairs() = {"EURGBP"};
    sensiData->fxVolShiftData()["EURGBP"] = fxvsData;

    sensiData->swaptionVolCurrencies() = {"EUR", "GBP"};
    sensiData->swaptionVolShiftData()["EUR"] = swvsData;
    sensiData->swaptionVolShiftData()["GBP"] = swvsData;

    sensiData->creditNames() = {"BondIssuer1"};
    sensiData->creditCurveShiftData()["BondIssuer1"] = cvsData;

    // sensiData->capFloorVolLabel() = "VOL_CAPFLOOR";
    // sensiData->capFloorVolCurrencies() = { "EUR", "GBP" };
    // sensiData->capFloorVolShiftData()["EUR"] = cfvsData;
    // sensiData->capFloorVolShiftData()["EUR"].indexName = "EUR-EURIBOR-6M";
    // sensiData->capFloorVolShiftData()["GBP"] = cfvsData;
    // sensiData->capFloorVolShiftData()["GBP"].indexName = "GBP-LIBOR-6M";

    return sensiData;
}

boost::shared_ptr<ore::analytics::SensitivityScenarioData> TestConfigurationObjects::setupSensitivityScenarioData2b() {
    boost::shared_ptr<ore::analytics::SensitivityScenarioData> sensiData =
        boost::make_shared<ore::analytics::SensitivityScenarioData>();

    // shift curve has more points than underlying curve, has tenor points where the underlying curve hasn't, skips some
    // tenor points that occur in the underlying curve (e.g. 2Y, 3Y, 4Y)
    ore::analytics::SensitivityScenarioData::CurveShiftData cvsData;
    cvsData.shiftTenors = {1 * Years,   15 * Months, 18 * Months, 21 * Months, 27 * Months, 30 * Months, 33 * Months,
                           42 * Months, 54 * Months, 5 * Years,   6 * Years,   7 * Years,   8 * Years,   9 * Years,
                           10 * Years,  11 * Years,  12 * Years,  13 * Years,  14 * Years,  15 * Years,  16 * Years,
                           17 * Years,  18 * Years,  19 * Years,  20 * Years};
    cvsData.shiftType = "Absolute";
    cvsData.shiftSize = 0.0001;

    ore::analytics::SensitivityScenarioData::SpotShiftData fxsData;
    fxsData.shiftType = "Relative";
    fxsData.shiftSize = 0.01;

    ore::analytics::SensitivityScenarioData::VolShiftData fxvsData;
    fxvsData.shiftType = "Relative";
    fxvsData.shiftSize = 1.0;
    fxvsData.shiftExpiries = {2 * Years, 5 * Years};

    ore::analytics::SensitivityScenarioData::CapFloorVolShiftData cfvsData;
    cfvsData.shiftType = "Absolute";
    cfvsData.shiftSize = 0.0001;
    cfvsData.shiftExpiries = {1 * Years, 2 * Years, 3 * Years, 5 * Years, 10 * Years};
    cfvsData.shiftStrikes = {0.05};

    ore::analytics::SensitivityScenarioData::SwaptionVolShiftData swvsData;
    swvsData.shiftType = "Relative";
    swvsData.shiftSize = 0.01;
    swvsData.shiftExpiries = {3 * Years, 5 * Years, 10 * Years};
    swvsData.shiftTerms = {2 * Years, 5 * Years, 10 * Years};

    sensiData->discountCurrencies() = {"EUR", "GBP"};
    sensiData->discountCurveShiftData()["EUR"] = cvsData;

    sensiData->discountCurveShiftData()["GBP"] = cvsData;

    sensiData->indexNames() = {"EUR-EURIBOR-6M", "GBP-LIBOR-6M"};
    sensiData->indexCurveShiftData()["EUR-EURIBOR-6M"] = cvsData;
    sensiData->indexCurveShiftData()["GBP-LIBOR-6M"] = cvsData;

    sensiData->yieldCurveNames() = {"BondCurve1"};
    sensiData->yieldCurveShiftData()["BondCurve1"] = cvsData;

    sensiData->fxCcyPairs() = {"EURGBP"};
    sensiData->fxShiftData()["EURGBP"] = fxsData;

    sensiData->fxVolCcyPairs() = {"EURGBP"};
    sensiData->fxVolShiftData()["EURGBP"] = fxvsData;

    sensiData->swaptionVolCurrencies() = {"EUR", "GBP"};
    sensiData->swaptionVolShiftData()["EUR"] = swvsData;
    sensiData->swaptionVolShiftData()["GBP"] = swvsData;

    sensiData->creditNames() = {"BondIssuer1"};
    sensiData->creditCurveShiftData()["BondIssuer1"] = cvsData;

    // sensiData->capFloorVolLabel() = "VOL_CAPFLOOR";
    // sensiData->capFloorVolCurrencies() = { "EUR", "GBP" };
    // sensiData->capFloorVolShiftData()["EUR"] = cfvsData;
    // sensiData->capFloorVolShiftData()["EUR"].indexName = "EUR-EURIBOR-6M";
    // sensiData->capFloorVolShiftData()["GBP"] = cfvsData;
    // sensiData->capFloorVolShiftData()["GBP"].indexName = "GBP-LIBOR-6M";

    return sensiData;
}

boost::shared_ptr<ore::analytics::SensitivityScenarioData> TestConfigurationObjects::setupSensitivityScenarioData5() {
    boost::shared_ptr<ore::analytics::SensitivityScenarioData> sensiData =
        boost::make_shared<ore::analytics::SensitivityScenarioData>();

    ore::analytics::SensitivityScenarioData::CurveShiftData cvsData;
    cvsData.shiftTenors = {6 * Months, 1 * Years,  2 * Years,  3 * Years, 5 * Years,
                           7 * Years,  10 * Years, 15 * Years, 20 * Years}; // multiple tenors: triangular shifts
    cvsData.shiftType = "Absolute";
    cvsData.shiftSize = 0.0001;

    ore::analytics::SensitivityScenarioData::SpotShiftData fxsData;
    fxsData.shiftType = "Relative";
    fxsData.shiftSize = 0.01;

    ore::analytics::SensitivityScenarioData::VolShiftData fxvsData;
    fxvsData.shiftType = "Relative";
    fxvsData.shiftSize = 1.0;
    fxvsData.shiftExpiries = {5 * Years};

    ore::analytics::SensitivityScenarioData::CapFloorVolShiftData cfvsData;
    cfvsData.shiftType = "Absolute";
    cfvsData.shiftSize = 0.0001;
    cfvsData.shiftExpiries = {1 * Years, 2 * Years, 3 * Years, 5 * Years, 10 * Years};
    cfvsData.shiftStrikes = {0.01, 0.02, 0.03, 0.04, 0.05};

    ore::analytics::SensitivityScenarioData::SwaptionVolShiftData swvsData;
    swvsData.shiftType = "Relative";
    swvsData.shiftSize = 0.01;
    swvsData.shiftExpiries = {2 * Years, 5 * Years, 10 * Years};
    swvsData.shiftTerms = {5 * Years, 10 * Years};

    ore::analytics::SensitivityScenarioData::SpotShiftData eqsData;
    eqsData.shiftType = "Relative";
    eqsData.shiftSize = 0.01;

    ore::analytics::SensitivityScenarioData::VolShiftData eqvsData;
    eqvsData.shiftType = "Relative";
    eqvsData.shiftSize = 0.01;
    eqvsData.shiftExpiries = {5 * Years};

    ore::analytics::SensitivityScenarioData::CurveShiftData zinfData;
    zinfData.shiftType = "Absolute";
    zinfData.shiftSize = 0.0001;
    zinfData.shiftTenors = { 1 * Years,  2 * Years,  3 * Years, 5 * Years,
        7 * Years,  10 * Years, 15 * Years, 20 * Years };

    sensiData->discountCurrencies() = {"EUR", "USD", "GBP", "CHF", "JPY"};
    sensiData->discountCurveShiftData()["EUR"] = cvsData;

    sensiData->discountCurveShiftData()["USD"] = cvsData;
    
    sensiData->discountCurveShiftData()["GBP"] = cvsData;

    sensiData->discountCurveShiftData()["JPY"] = cvsData;

    sensiData->discountCurveShiftData()["CHF"] = cvsData;

    sensiData->indexNames() = {"EUR-EURIBOR-6M", "USD-LIBOR-3M", "GBP-LIBOR-6M", "CHF-LIBOR-6M", "JPY-LIBOR-6M"};
    sensiData->indexCurveShiftData()["EUR-EURIBOR-6M"] = cvsData;

    sensiData->indexCurveShiftData()["USD-LIBOR-3M"] = cvsData;

    sensiData->indexCurveShiftData()["GBP-LIBOR-6M"] = cvsData;

    sensiData->indexCurveShiftData()["JPY-LIBOR-6M"] = cvsData;

    sensiData->indexCurveShiftData()["CHF-LIBOR-6M"] = cvsData;

    sensiData->yieldCurveNames() = {"BondCurve1"};
    sensiData->yieldCurveShiftData()["BondCurve1"] = cvsData;

    sensiData->creditNames() = {"BondIssuer1"};
    sensiData->creditCurveShiftData()["BondIssuer1"] = cvsData;

    sensiData->fxCcyPairs() = {"EURUSD", "EURGBP", "EURCHF", "EURJPY"};
    sensiData->fxShiftData()["EURUSD"] = fxsData;
    sensiData->fxShiftData()["EURGBP"] = fxsData;
    sensiData->fxShiftData()["EURJPY"] = fxsData;
    sensiData->fxShiftData()["EURCHF"] = fxsData;

    sensiData->fxVolCcyPairs() = {"EURUSD", "EURGBP", "EURCHF", "EURJPY", "GBPCHF"};
    sensiData->fxVolShiftData()["EURUSD"] = fxvsData;
    sensiData->fxVolShiftData()["EURGBP"] = fxvsData;
    sensiData->fxVolShiftData()["EURJPY"] = fxvsData;
    sensiData->fxVolShiftData()["EURCHF"] = fxvsData;
    sensiData->fxVolShiftData()["GBPCHF"] = fxvsData;

    sensiData->swaptionVolCurrencies() = {"EUR", "USD", "GBP", "CHF", "JPY"};
    sensiData->swaptionVolShiftData()["EUR"] = swvsData;
    sensiData->swaptionVolShiftData()["GBP"] = swvsData;
    sensiData->swaptionVolShiftData()["USD"] = swvsData;
    sensiData->swaptionVolShiftData()["JPY"] = swvsData;
    sensiData->swaptionVolShiftData()["CHF"] = swvsData;

    sensiData->capFloorVolCurrencies() = {"EUR", "USD"};
    sensiData->capFloorVolShiftData()["EUR"] = cfvsData;
    sensiData->capFloorVolShiftData()["EUR"].indexName = "EUR-EURIBOR-6M";
    sensiData->capFloorVolShiftData()["USD"] = cfvsData;
    sensiData->capFloorVolShiftData()["USD"].indexName = "USD-LIBOR-3M";

    sensiData->equityNames() = {"SP5", "Lufthansa"};
    sensiData->equityShiftData()["SP5"] = eqsData;
    sensiData->equityShiftData()["Lufthansa"] = eqsData;

    sensiData->equityVolNames() = {"SP5", "Lufthansa"};
    sensiData->equityVolShiftData()["SP5"] = eqvsData;
    sensiData->equityVolShiftData()["Lufthansa"] = eqvsData;

    sensiData->equityForecastCurveNames() = { "SP5", "Lufthansa" };
    sensiData->equityForecastCurveShiftData()["SP5"] = cvsData;
    sensiData->equityForecastCurveShiftData()["Lufthansa"] = cvsData;
    sensiData->zeroInflationIndices() = { "UKRPI" };
    sensiData->zeroInflationCurveShiftData()["UKRPI"] = zinfData;

    sensiData->yoyInflationIndices() = { "UKRPI" };
    sensiData->yoyInflationCurveShiftData()["UKRPI"] = zinfData;

    return sensiData;
}

} // namespace testsuite
