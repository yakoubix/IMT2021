
#include "binomialengine.hpp"
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/exercise.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/termstructures/volatility/equityfx/blackvariancecurve.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <iostream>
#include <chrono>
#ifdef BOOST_MSVC
#include <ql/auto_link.hpp>
#endif

using namespace QuantLib;

int main() {

    try {

        // modify the sample code below to suit your project
        Calendar calendar = TARGET();
        Date today = Date(24, February, 2021);
        Settings::instance().evaluationDate() = today;

        Option::Type type(Option::Put);
        Real underlying = 36;
        Real strike = 40;
        Date maturity(24, May, 2021);

        ext::shared_ptr<Exercise> americanExercise(new AmericanExercise(today, maturity));
        ext::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));

        Handle<Quote> underlyingH(ext::make_shared<SimpleQuote>(underlying));

        DayCounter dayCounter = Actual365Fixed();
        Handle<YieldTermStructure> riskFreeRate(
            ext::shared_ptr<YieldTermStructure>(
                new ZeroCurve({today, today + 6*Months}, {0.01, 0.015}, dayCounter)));
        Handle<BlackVolTermStructure> volatility(
            ext::shared_ptr<BlackVolTermStructure>(
                new BlackVarianceCurve(today, {today+3*Months, today+6*Months}, {0.20, 0.25}, dayCounter)));

        ext::shared_ptr<BlackScholesProcess> bsmProcess(
                 new BlackScholesProcess(underlyingH, riskFreeRate, volatility));

        // options
        VanillaOption americanOption(payoff, americanExercise);

        Size timeSteps = 100;
        bool b1 = false;
        ext::shared_ptr<PricingEngine> engine(
                new BinomialVanillaEngine_2<JarrowRudd>(bsmProcess,timeSteps,b1));
        americanOption.setPricingEngine(engine);

        auto startTime = std::chrono::steady_clock::now();

        Real NPV = americanOption.NPV();

        auto endTime = std::chrono::steady_clock::now();

        double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        std::cout << "NPV: " << NPV << std::endl;
        std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;

        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}

