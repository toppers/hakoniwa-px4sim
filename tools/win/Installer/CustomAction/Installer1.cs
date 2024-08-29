using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Threading.Tasks;

namespace CustomAction
{
    [RunInstaller(true)]
    public partial class Installer1 : System.Configuration.Install.Installer
    {
        public override void Install(System.Collections.IDictionary stateSaver)
        {
            // Install後の動作
            base.Install(stateSaver);

            // 環境変数PATHの追加
            string currentPath;
            currentPath = System.Environment.GetEnvironmentVariable("path", System.EnvironmentVariableTarget.User);
            string installPath = this.Context.Parameters["InstallPath"];
            string path = installPath + @"\hakoniwa-px4-win\bin;";
            
            
            if (currentPath == null)
            {
                currentPath = path;
            }
            else if (currentPath.EndsWith(";"))
            {
                currentPath += path;
            }
            else
            {
                currentPath += ";"+path;
            }

            // 環境変数PATHを設定する
            System.Environment.SetEnvironmentVariable("path", currentPath, System.EnvironmentVariableTarget.User);

            //hakoniwa windows HAKO_BINARY_PATHの設定
            string binpath = installPath + @"\hakoniwa-px4-win\hakoniwa\py\hako_binary\offset";
            System.Environment.SetEnvironmentVariable("HAKO_BINARY_PATH", binpath, System.EnvironmentVariableTarget.User);

#if DEBUG
            System.Windows.Forms.MessageBox.Show(currentPath);
            System.Windows.Forms.MessageBox.Show(path);
            System.Windows.Forms.MessageBox.Show(binpath);
#endif
            //hakoniwa windows HAKO_CONFIG_PATHの設定
            int hako_config = int.Parse(this.Context.Parameters["hakoconfig"]);
            string configpath ;

            if (hako_config == 1)
            {
                configpath = installPath + @"\hakoniwa-px4-win\hakoniwa\config\cpp_core_config.json";
            }
            else if (hako_config == 2)
            {
                configpath = installPath + @"\hakoniwa-px4-win\hakoniwa\config_api\cpp_core_config.json";
            }
            else if (hako_config == 3)
            {
                configpath = installPath + @"\hakoniwa-px4-win\hakoniwa\config_api2\cpp_core_config.json";
            }
            else
            {
                configpath = installPath + @"Error config mode!!";
            }
#if DEBUG
            System.Windows.Forms.MessageBox.Show(configpath);
#endif


            System.Environment.SetEnvironmentVariable("HAKO_CONFIG_PATH", configpath, System.EnvironmentVariableTarget.User);
#if DEBUG
            System.Windows.Forms.MessageBox.Show(configpath);
#endif
            // PYTHONPATHの設定
            string pythonPath;

            pythonPath = System.Environment.GetEnvironmentVariable("PYTHONPATH", System.EnvironmentVariableTarget.User);
            string hakopypath = installPath + @"\hakoniwa-px4-win\hakoniwa\py;";
#if DEBUG
            System.Windows.Forms.MessageBox.Show(hakopypath);
#endif

            if (pythonPath == null)
            {
                pythonPath = hakopypath;
            }
            else if (pythonPath.EndsWith(";"))
            {
                pythonPath += hakopypath;
            }
            else
            {
                pythonPath += ";" + hakopypath;
            }

            // 環境変数PATHを設定する
            System.Environment.SetEnvironmentVariable("PYTHONPATH", pythonPath, System.EnvironmentVariableTarget.User);

#if DEBUG
            System.Windows.Forms.MessageBox.Show(pythonPath);
#endif







        }

        //public override void Commit(System.Collections.IDictionary savedState)
        //{
        //    //コミット動作
        //    base.Commit(savedState);
        //    System.Windows.Forms.MessageBox.Show(“Commit”);
        //}

        //public override void Rollback(System.Collections.IDictionary savedState)
        //{
        //    //失敗時のロールバック動作
        //    base.Rollback(savedState);
        //    System.Windows.Forms.MessageBox.Show(“Rollback”);
        //}

        public override void Uninstall(System.Collections.IDictionary savedState)
        {
            //Un-install動作
            base.Uninstall(savedState);

            
            // 環境変数PATHを編集
            string currentPath;
            currentPath = System.Environment.GetEnvironmentVariable("path", System.EnvironmentVariableTarget.User);
            string installPath = this.Context.Parameters["InstallPath"];
            string path = installPath + @"\hakoniwa-px4-win\bin;";
            currentPath = currentPath.Replace(path, "");
            
            // 環境変数PATHから削除する
            System.Environment.SetEnvironmentVariable("path", currentPath, System.EnvironmentVariableTarget.User);
#if DEBUG
            System.Windows.Forms.MessageBox.Show(currentPath);
            System.Windows.Forms.MessageBox.Show(path);
#endif

            // PYTHONPATHから箱庭関連を消す
            string pythonPath = System.Environment.GetEnvironmentVariable("PYTHONPATH", System.EnvironmentVariableTarget.User);
            string hakopypath = installPath + @"\hakoniwa-px4-win\hakoniwa\py;";
            pythonPath = pythonPath.Replace(hakopypath, "");
#if DEBUG
            System.Windows.Forms.MessageBox.Show(hakopypath);
#endif
            // PYTHONPATHから箱庭関連を削除
            System.Environment.SetEnvironmentVariable("PYTHONPATH", pythonPath, System.EnvironmentVariableTarget.User);

#if DEBUG
            System.Windows.Forms.MessageBox.Show(pythonPath);
            System.Windows.Forms.MessageBox.Show(hakopypath);
#endif

            // hakoniwa関連のPATHを削除する
            System.Environment.SetEnvironmentVariable("HAKO_BINARY_PATH", "", System.EnvironmentVariableTarget.User);
            System.Environment.SetEnvironmentVariable("HAKO_CONFIG_PATH", "", System.EnvironmentVariableTarget.User);



        }





    }
}
