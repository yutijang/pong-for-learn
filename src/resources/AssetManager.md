Kiến trúc của `AssetManager`:

`assetId` chỉ là một bí danh (alias) được sử dụng bởi `Scene`.
Ví dụ:
font_menu_label -> assets/fonts/roboto.ttf
font_label -> assets/fonts/roboto.ttf

Các tài nguyên (resources) được lưu vào bộ nhớ đệm (cached) và quản lý bằng cơ chế đếm phân đoạn tham chiếu (reference-counted) dựa trên đường dẫn tệp vật lý.
Vì vậy, một tệp tài nguyên chỉ được tải duy nhất một lần, ngay cả khi có nhiều `assetId` cùng tham chiếu đến một tệp đó.

Luồng tra cứu (Lookup flow):
assetId
-> AssetDesc
-> đường dẫn tài nguyên (resource path)
-> tài nguyên trong bộ nhớ đệm (cached resource)

`Scene` luôn truy cập các tài nguyên thông qua `assetId`.
`AssetManager` sẽ tự động phân giải (resolve) `assetId` thành tài nguyên dùng chung (shared resource) ở bên trong hệ thống.



---

Đây chính là điểm mấu chốt của kiến trúc này.

---

## Dữ kiện

Giả sử:

### Registry

```text id="a1"
font_menu_label
    →
(path=assets/fonts/roboto.ttf)

font_label
    →
(path=assets/fonts/roboto.ttf)
```

---

### Cache

```text id="a2"
m_fonts

assets/fonts/roboto.ttf
    →
Font object
```

---

## Khi Scene gọi

```cpp id="a3"
assetManager.font("font_menu_label");
```

---

### Bước 1

Tra cứu:

```cpp id="a4"
m_assetRegistry["font_menu_label"]
```

kết quả:

```text id="a5"
path = assets/fonts/roboto.ttf
```

---

### Bước 2

Lấy path:

```cpp id="a6"
auto const& path = desc.path;
```

---

### Bước 3

Tra cache:

```cpp id="a7"
m_fonts[path]
```

tức là:

```cpp id="a8"
m_fonts["assets/fonts/roboto.ttf"]
```

---

### Bước 4

Trả về:

```cpp id="a9"
return &fontIt->second;
```

---

## Điều quan trọng

Scene không biết:

```text id="a10"
roboto.ttf
```

Scene chỉ biết:

```text id="a11"
font_menu_label
```

---

AssetManager chịu trách nhiệm dịch:

```text id="a12"
font_menu_label
```

thành:

```text id="a13"
assets/fonts/roboto.ttf
```

---

## Hình dung giống DNS

Giống như:

```text id="a14"
google.com
```

không phải IP.

---

Trình duyệt làm:

```text id="a15"
google.com
    →
142.251.xxx.xxx
```

---

Tương tự:

```text id="a16"
font_menu_label
    →
assets/fonts/roboto.ttf
    →
Font object
```

---

## Ưu điểm

Sau này đổi font:

```text id="a17"
roboto.ttf
```

sang:

```text id="a18"
roboto_bold.ttf
```

chỉ sửa:

```text id="a19"
asset registry
```

---

Toàn bộ Scene:

```cpp id="a20"
font("font_menu_label");
```

không cần sửa.

---

## Một điểm rất đáng chú ý

Khi:

```cpp id="a21"
font("font_menu_label");
```

được gọi,

AssetManager **không load gì cả**.

Nó chỉ:

```text id="a22"
tra cứu
```

---

Việc load đã xảy ra trước đó trong:

```cpp id="a23"
loadAssetGroup(...)
```

---

Cho nên:

```cpp id="a24"
font()
```

nên là:

```text id="a25"
lookup API
```

không phải:

```text id="a26"
load API
```

---

## Luồng đầy đủ

```text id="a27"
loadAssetGroup("menu")

    ↓

font_menu_label

    ↓

roboto.ttf

    ↓

LoadFont()

    ↓

m_fonts[roboto.ttf]

------------------------------------------------

Scene

    ↓

font("font_menu_label")

    ↓

m_assetRegistry

    ↓

roboto.ttf

    ↓

m_fonts[roboto.ttf]

    ↓

Font object
```

Đây là lý do:

```text id="a28"
Scene làm việc bằng assetId

nhưng runtime chỉ tồn tại một Font object cho mỗi file vật lý.
```
