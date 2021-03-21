
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
#include <fstream>
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
        Size timeSteps_maxi = 600;
        std::map<int, Real> summary_table;
        std::map<int, Real> summary_table_n;

        std::cout << "Binomial Tree with oscillations resolution scheme" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << "|" << " Time Step " << " |" << " Value " << "|" << " Elapsed Time " << std::endl;
        const char* path1 = "./data_cpp4.txt";
        std::ofstream myfile1;
        myfile1.open(path1);
        for (int step = 3; step < timeSteps_maxi; step++) {
            bool b1_oscillations = true;
            ext::shared_ptr<PricingEngine> engine(
                new BinomialVanillaEngine_2<AdditiveEQPBinomialTree>(bsmProcess, step, b1_oscillations));
            americanOption.setPricingEngine(engine);

            auto startTime = std::chrono::steady_clock::now();

            Real NPV = americanOption.NPV();

            auto endTime = std::chrono::steady_clock::now();

            double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
            summary_table[step] = NPV;
            Real step_to_write = step;
            Real value_to_write = NPV;
            myfile1 << step_to_write << " " << value_to_write << std::endl;
            std::cout << "|" << step << "           " << "|" << summary_table[step] << "|" << us / 1000000 << " s" << std::endl;
            std::cout << "----------------------------------" << std::endl;
            //std::cout << "NPV: " << NPV << std::endl;
            //std::cout << "Elapsed time: " << us / 1000000 << " s" << std::endl;
        }
        myfile1.close();

        const char* path2 = "C:/Users/Yakoubi/IMT2021/project4/data_cpp2.txt";
        std::ofstream myfile2;
        myfile2.open(path2);
        std::cout << "Binomial Tree without oscillations resolution scheme" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << "|" << " Time Step " << " |" << " Value " << "|" << " Elapsed Time " << std::endl;
        for (int step = 3; step < timeSteps_maxi; step++) {
            bool b2_noscillations = false;
            ext::shared_ptr<PricingEngine> engine2(
                new BinomialVanillaEngine_2<AdditiveEQPBinomialTree>(bsmProcess, step, b2_noscillations));
            americanOption.setPricingEngine(engine2);

            auto startTime = std::chrono::steady_clock::now();

            Real NPV = americanOption.NPV();

            auto endTime = std::chrono::steady_clock::now();

            double us = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
            summary_table_n[step] = NPV;
            Real step_to_write = step;
            Real value_to_write = NPV;
            myfile2 << step_to_write << " " << value_to_write << std::endl;
            std::cout << "|" << step << "           " << "|" << summary_table_n[step] << "|" << us / 1000000 << " s" << std::endl;
            std::cout << "----------------------------------" << std::endl;
        }
        myfile2.close();
        return 0;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}

