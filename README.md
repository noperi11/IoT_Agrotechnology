# IoT Agrotechnology

![IoT Agrotechnology](https://github.com/user-attachments/assets/21bb8f15-3034-4fe9-9b30-eee9a6de7491)
<img width="1168" height="816" alt="image" src="https://github.com/user-attachments/assets/5ba6529b-2c96-4756-8de4-13d3a2f2a0f2" />


# 🌾 IoT Pertanian Cerdas Berbasis ESP32

Proyek ini mengimplementasikan **teknologi IoT di bidang pertanian** untuk meningkatkan efisiensi, produktivitas, dan keberlanjutan pertanian. Sistem menggunakan **ESP32** yang terhubung ke internet untuk mengumpulkan, mengelola, dan mengirimkan data lingkungan ke **ThingsBoard** serta menampilkan data secara lokal melalui **LCD I2C** dan **Web Server**.

## 🚀 Fitur Utama

- ✅ Monitoring suhu dan kelembapan udara (DHT21)
- ✅ Monitoring kelembapan tanah (Soil Moisture)
- ✅ Monitoring tinggi air (HC-SR04)
- ✅ Pengiriman data ke ThingsBoard via MQTT
- ✅ Tampilan data di LCD I2C
- ✅ Tampilan data di Web Server lokal
- ✅ Pengendalian pompa dan aktuator melalui relay
- ✅ Indikator LED (Merah, Kuning, Hijau) untuk tinggi air

---

## 🧰 Perangkat yang Digunakan

| Perangkat            | Kegunaan                                      |
|----------------------|-----------------------------------------------|
| **ESP32**            | Mikrokontroler utama                          |
| **LCD I2C**          | Menampilkan suhu, kelembapan, dan soil        |
| **DHT21**            | Sensor suhu dan kelembapan udara              |
| **Soil Moisture**    | Sensor kelembapan tanah                       |
| **HC-SR04**          | Sensor ultrasonik untuk mengukur tinggi air   |
| **Relay (3 buah)**   | Mengontrol pompa dan aktuator lainnya         |
| **Pompa Air**        | Mengairi tanaman                              |
| **LED RGB (3 warna)**| Indikator tinggi air (Merah, Kuning, Hijau)   |

---

## 🖥️ Pin ESP32 yang Digunakan

| Perangkat               | Pin ESP32     |
|-------------------------|---------------|
| **DHT21**               | GPIO 4        |
| **Soil Moisture**       | GPIO 34 (ADC) |
| **Relay 1**             | GPIO 26       |
| **Relay 2**             | GPIO 25       |
| **Relay 3 (Pompa)**     | GPIO 17       |
| **LCD I2C (SDA/SCL)**   | SDA (GPIO 21), SCL (GPIO 22) |
| **HC-SR04 (TRIG)**      | GPIO 18       |
| **HC-SR04 (ECHO)**      | GPIO 5        |
| **LED Merah**           | GPIO 21       |
| **LED Kuning**          | GPIO 22       |
| **LED Hijau**           | GPIO 23       |

---

## 🌐 Integrasi ThingsBoard

Data yang dikirim ke ThingsBoard meliputi:

- Suhu (`Suhu`)
- Kelembapan Udara (`Kelembaban`)
- Kelembapan Tanah (`Soil Moisture`)
- Tinggi Air (`Tinggi Air`)

Contoh payload yang dikirim ke ThingsBoard:

```json
{
  "Suhu": 29.4,
  "Kelembaban": 66.2,
  "Tinggi Air": 102.3,
  "Soil Moisture": 87.0
}
