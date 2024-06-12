# BULANIK-MANTIK-FUZZY-LOGIC-ile3-EKSEN-GIMBAL-KONTROLCU-SISTEMI
Günümüzde görüntü teknolojisinin kullanımı askeri-sivil alanlarda fotoğraf ve video görüntülemesi olarak yaygınlaşmıştır. Bunlar kamera sistemleri ile sağlanmaktadır. Bu kamera sistemleri özellikle video çekimlerinde gözle görülür görüntülerde bozulma olarak yansımaktadır. Bunun önüne geçilmesi için çalışmalar/araştırmalar yapılmış ve günümüzde en yaygı ve kararlı olarak çalışan Gimbal sistemi kullanılmaktadır.  Bu sistem, yerçekimi kuvvetine göre tıpkı bir su terazisi gibi fiziksel ve donanımsal olarak eksenlerini sıfır noktasında tutmaya çalışır. Burada eksenerin arttırılması ile Gimbal sistemi kamera merkezinin daha kararlı ve istenilen sonuçlarda sıfır noktasını stabil şekilde yakalayarak konumunu burada korumasını sağlar.
Gimbal sistemlerinin, özellikle İHA’larda görev gerçekleştirme konusunda yüksek öneme sahip olduğu gözlemlenmiş ve bu alanda tasarım ve üretim planlanmıştır. Bunun neticesinde, bir görüntüde bulunan nesnelere kitlenebilme özelliği katabilmek adına bu sistemlere uygun olarak entegre edilebilmesi için malzeme seçimi, kod düzenlemesi ve haberleşme mantığına uygun olarak çalışma gerçekleşmiştir. Basit düşünülerek, kitleme verilerini üretecek herhangi bir işlemci karttan UART haberleşmesi ile “x-y-z” olarak veri alınabilir şekilde kod düzenlemesi gerçekleşmiştir.
Proje çalışmasında, 2 eksenli gimballere göre daha karmaşık fakat daha stabil sonuç vermesinden dolayı 3 eksenli gimbal tercih edilmiştir. Bununla beraber sisteme uygun IMU sensörler incelenerek aralarında en kararlı sonucu veren MPU9250 sensör kullanımı araştırmalar ve denemeler ile kararlaştırılmıştır. Sonrasında bu sensörden alınan verilerin gürültülü olması ve sistemi kararsız hale getireceği için Kalman filtre uygulanmış ve gerçeğe uygun açısal değerler 3 eksende üretimi sağlanmıştır.
Sistemin kamera merkezinin sıfır noktasına göre hatalı açı değerlerini ölçmek için 2.IMU sensör Arduino Nano üzerinden Arduino Mega ana işlemciye UART yoluyla 3 eksende gönderilmektedir.
Ana işlemcide 1.IMU sensörden 3 eksende referans sinyali olarak üretilmektedir. Burada referans sinyali, Bulanık Mantık kontrolcüye girmektedir. Ayrıca kontrolcüye hata açı değerini ölçmek için kullanılan 2.IMU sensöre verileride girdi olarak kullanılmıştır. Burada kümelere ayrılmış şekilde kontrolcü mantığı uygulanmış ve motorları sürecek olan PWM sinyali üretimi sağlanmıştır. Sinyalin hata oranına göre değişim hızları kontrolü sağlanmıştır. Prototip üzerinde de çalışması gerçekleşmiş ve yeterliliği test edilmiştir.
Anahtar kelimeler: Bulanık Mantık, Gimbal, IMU, Kalman Filtre, PWM, Servo Motor

![image](https://github.com/karanizeybek/BULANIK-MANTIK-FUZZY-LOGIC-ile3-EKSEN-GIMBAL-KONTROLCU-SISTEMI/assets/76970557/958313dc-b121-4576-bf09-a058aeb984d1)

![image](https://github.com/karanizeybek/BULANIK-MANTIK-FUZZY-LOGIC-ile3-EKSEN-GIMBAL-KONTROLCU-SISTEMI/assets/76970557/945c0581-8c3e-4775-8e35-770be2bd3ae7)

![Arduino Mega](https://github.com/karanizeybek/BULANIK-MANTIK-FUZZY-LOGIC-ile3-EKSEN-GIMBAL-KONTROLCU-SISTEMI/assets/76970557/ac0f7899-4332-4511-9e00-103f4076a898)

![image](https://github.com/karanizeybek/BULANIK-MANTIK-FUZZY-LOGIC-ile3-EKSEN-GIMBAL-KONTROLCU-SISTEMI/assets/76970557/4030aec1-f4b6-4105-82e4-140660509397)

![image](https://github.com/karanizeybek/BULANIK-MANTIK-FUZZY-LOGIC-ile3-EKSEN-GIMBAL-KONTROLCU-SISTEMI/assets/76970557/6aa82f44-f614-4333-a8e0-22724d307cf6)

Demo; https://youtube.com/shorts/ZXwSB-o7VV0


