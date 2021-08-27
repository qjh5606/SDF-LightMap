# SDF-LightMap

实现了基于SDF的卡通阴影图生成。

- SDF的生成采用8ssedt算法。

- 利用生成的SDF图进行插值，得到最后的阴影图。

具体的实现说明可以查看博客 [基于SDF的卡通阴影图生成](https://blog.csdn.net/qjh5606/article/details/119958786?spm=1001.2014.3001.5501)。


示例输入：

![美术绘制的多张中间过程图](https://img-blog.csdnimg.cn/f6d7b55174ef4a04ab2c60080d350ad1.png#pic_center)

输出结果：

- 注，这里显示的就是线性的数值。而非Gamma SRGB。（笔者在输出图像前先作了逆Gamma校正）。

![算法结果](https://img-blog.csdnimg.cn/65d33cade75e4d3683b1b7a783aa1edd.jpg?x-oss-process=image/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBA5qGR5p2lOTM=,size_20,color_FFFFFF,t_70,g_se,x_16#pic_center)





