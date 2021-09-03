/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/path_service.h"
#include "base/task/post_task.h"
#include "base/test/bind.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/thread_test_helper.h"
#include "brave/browser/brave_wallet/brave_wallet_tab_helper.h"
#include "brave/browser/brave_wallet/rpc_controller_factory.h"
#include "brave/common/brave_paths.h"
#include "brave/components/brave_wallet/browser/eth_json_rpc_controller.h"
#include "brave/components/brave_wallet/common/features.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "net/dns/mock_host_resolver.h"

namespace {

const char kEmbeddedTestServerDirectory[] = "brave-wallet";

const char kScriptWaitForEvent[] = R"(
    function waitForEvent() {
        if (chain_added_result) {
          console.log('!!!send true')
          window.domAutomationController.send(true);
        }
    }
    console.log('!!!starting')
    setInterval(waitForEvent, 100);)";

const char kScriptAddBinanceChainErrorRejected[] = R"(
      function waitForEvent() {
        if (!window.ethereum)
          return;
        window.ethereum.request({ method: 'wallet_addEthereumChain', params:[{
              chainId: '0x38',
              chainName: 'Binance1 Smart Chain',
              rpcUrls: ['https://bsc-dataseed.binance.org/'],
          }]
        }).catch(result => {
          window.domAutomationController.send((result.error && result.error.code == 4001))
        })
      };
      console.log("!!!starting");
      setInterval(waitForEvent, 100);
    )";

}  // namespace

class BraveWalletEthereumChainTest : public InProcessBrowserTest {
 public:
  BraveWalletEthereumChainTest() {
    feature_list_.InitAndEnableFeature(
        brave_wallet::features::kNativeBraveWalletFeature);
  }

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();
    host_resolver()->AddRule("*", "127.0.0.1");

    https_server_.reset(new net::EmbeddedTestServer(
        net::test_server::EmbeddedTestServer::TYPE_HTTPS));
    https_server_->SetSSLConfig(net::EmbeddedTestServer::CERT_OK);

    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    test_data_dir = test_data_dir.AppendASCII(kEmbeddedTestServerDirectory);
    https_server_->ServeFilesFromDirectory(test_data_dir);

    ASSERT_TRUE(https_server_->Start());
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    // HTTPS server only serves a valid cert for localhost, so this is needed
    // to load pages from other hosts without an error.
    command_line->AppendSwitch(switches::kIgnoreCertificateErrors);
  }

  net::EmbeddedTestServer* https_server() { return https_server_.get(); }

  mojo::Remote<brave_wallet::mojom::EthJsonRpcController>
  GetEthJsonRpcController() {
    if (!rpc_controller_) {
      auto pending =
          brave_wallet::RpcControllerFactory::GetInstance()->GetForContext(
              browser()->profile());
      rpc_controller_.Bind(std::move(pending));
    }
    return std::move(rpc_controller_);
  }

 private:
  mojo::Remote<brave_wallet::mojom::EthJsonRpcController> rpc_controller_;
  std::unique_ptr<net::EmbeddedTestServer> https_server_;
  base::test::ScopedFeatureList feature_list_;
};

IN_PROC_BROWSER_TEST_F(BraveWalletEthereumChainTest, AddEthereumChain) {
  GURL url =
      https_server()->GetURL("a.com", "/brave_wallet_ethereum_chain.html");
  ui_test_utils::NavigateToURL(browser(), url);
  content::WebContents* contents =
      browser()->tab_strip_model()->GetActiveWebContents();
  WaitForLoadStop(contents);
  ASSERT_TRUE(brave_wallet::BraveWalletTabHelper::FromWebContents(contents)
                  ->IsShowingBubble());
  GetEthJsonRpcController()->PendingRequestCompleted("0x38", true);
  auto result_first = EvalJs(contents, kScriptWaitForEvent,
                             content::EXECUTE_SCRIPT_USE_MANUAL_REPLY);
  EXPECT_EQ(base::Value(true), result_first.value);
}

IN_PROC_BROWSER_TEST_F(BraveWalletEthereumChainTest, AddEthereumChainAgain) {
  GURL url =
      https_server()->GetURL("a.com", "/brave_wallet_ethereum_chain.html");
  ui_test_utils::NavigateToURL(browser(), url);
  content::WebContents* contents =
      browser()->tab_strip_model()->GetActiveWebContents();
  WaitForLoadStop(contents);
  auto* tab_helper =
      brave_wallet::BraveWalletTabHelper::FromWebContents(contents);
  ASSERT_TRUE(tab_helper->IsShowingBubble());
  tab_helper->CloseBubble();
  base::RunLoop().RunUntilIdle();
  ASSERT_FALSE(tab_helper->IsShowingBubble());
  auto result_first = EvalJs(contents, kScriptAddBinanceChainErrorRejected,
                             content::EXECUTE_SCRIPT_USE_MANUAL_REPLY);
  ASSERT_FALSE(tab_helper->IsShowingBubble());
  EXPECT_EQ(base::Value(true), result_first.value);
}
