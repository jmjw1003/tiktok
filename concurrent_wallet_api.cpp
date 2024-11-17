#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <future>
#include <map>

// Functions to simulate balance & voucher data fetches
double fetchBalance(int uid) {
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    return 150.43;
}

double fetchBalanceFail(int uid) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 150.43;
}

double fetchVoucher(int uid) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return -99.4;
}

double fetchVoucherFail(int uid) {
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    return -99.4;
}

// Function wrapper to execute balance & voucher fetches within 50ms
std::map<std::string, double> getWalletInfo(
    int uid,
    std::function<double(int)> fetchBalance,
    std::function<double(int)> fetchVoucher,
    std::chrono::milliseconds timeout = std::chrono::milliseconds(50))
{
    std::map<std::string, double> walletInfo;
    try {
        // Run both balance & voucher calls concurrently using std::async
        auto balanceFuture = std::async(std::launch::async, fetchBalance, uid);
        auto voucherFuture = std::async(std::launch::async, fetchVoucher, uid);

        // Wait for fetchBalance to complete
        if (balanceFuture.wait_for(timeout) == std::future_status::timeout) {
            // Strong dependency -> throw an error if timeout is reached
            throw std::runtime_error("Wallet balance could not be fetched within time limit");
        }
        walletInfo["Balance"] = balanceFuture.get();

        // Wait for fetchVoucher to complete (weak dependency -> optional)
        if (voucherFuture.wait_for(timeout) == std::future_status::ready) {
            walletInfo["Voucher"] = voucherFuture.get();
        }
    }
    catch (const std::exception &e) {
        std::cout << "Exception: " << e.what() << '\n';
    }
    return walletInfo;
}


int main() {
    int uid = 12345;

    // Make an API call for full wallet info
    auto bothPass = getWalletInfo(uid, fetchBalance, fetchVoucher);

    // Output the results
    std::cout << "Full wallet details API call\n";
    for (auto i : bothPass) {
        std::cout << i.first << ": " << i.second << '\n';
    }

    // Make an API call for partial wallet info
    std::cout << "\nBalance only API call\n";
    auto balanceOnly = getWalletInfo(uid, fetchBalance, fetchVoucherFail);

    // Output the results
    for (auto i : balanceOnly) {
        std::cout << i.first << ": " << i.second << '\n';
    }

    // Make an API call for no wallet info
    std::cout << "\nWallet API fail\n";
    auto fail = getWalletInfo(uid, fetchBalanceFail, fetchVoucher);

    // Output the results
    for (auto i : fail) {
        std::cout << i.first << ": " << i.second << '\n';
    }

    return 0;
}

/*
Program output:
Full wallet details API call
Balance: 150.43
Voucher: -99.4

Balance only API call
Balance: 150.43

Wallet API fail
Exception: Wallet balance could not be fetched within time limit
*/