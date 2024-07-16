1. 容器日志引擎改为journald
2. fix bug：当安装包名字和安装包内/bin目录下实际bin文件名字不一致时，appm -I 时，app状态显示异常
3. 添加ssal验签接口，container/appm安装/启动APP时，会进行APP的验签。会打印验签结果，但是验签失败后，并不退出，程序正常运行。
4. 去除APP监控周期性mem/cpu使用率打印，只在越限和越限恢复时打印
5. container扩充buffer，防止因为过多 -v/-dev 参数时，参数丢失
6. container增强运行过程中的防御性检测，有异常时会主动打印失败原因

20220827 add:
7. 容器container -p端口映射记录按照最新规范重新匹配
8. 取消appSignTool_arm, 使用主动覆盖的方式阻止非法APP启动，此功能需要更新container和容器基础镜像，已安装的容器也需要重新安装
9. 添加容器内配置文件的主动防护（配置文件大小为0/文件格式损坏）

20220828 add:
10. ResMonitor监控容器的准备时间从120s缩减为60s，即ResMonitor启动后的前60s不执行容器资源监控，ResMonitor启动后，若新安装一个容器，则从安装完容器开始，60s内不监控此新增容器，这60s内其他容器正常监控。
11. 容器基础镜像更新到20220828，SecRenif解压已验签APP安装包后，安装包存放路径更新为/backup/app_back/app_name/，原先为/backup/app_back/
12. 缩短appm -i/container install with APP两种方式安装APP时间

20220829 add:
13. 安装包存放路径更新为/backup/app_back/app_name/bin
14. 更新container，容器带APP安装时，安装完容器要延时2s，等待appm_docker链接MQTT，否则会概率性失败；

20220830 add:
15. ssl验签结果测试完毕，已经能从SecRenif成功获取验签结果。
16. 收紧License/Signature检测结果判断，验证失败则启动/安装APP失败

20220905 add:
17. License验证，HAL Licnese读取接口更新，int (*devinfo_read_applicense_index)(struct tag_DEVINFO_DEVICE *dev, uint32 index, char *appno, char *appname, char *license, int max_license_size);


