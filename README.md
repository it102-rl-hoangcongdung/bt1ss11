# BÁO CÁO PHÂN TÍCH LỖI LOGIC HỆ THỐNG KIỂM ĐỊNH AN TOÀN KHO HÀNG WMS

> 👤 **Học viên:** Hoàng Công Dũng | **Mã SV:** PTIT-HCM-009
> 🏫 **Môn học:** it102-rl-hoangcongdung

---

## 📊 Sơ đồ thiết kế hệ thống (Flowchart)

> 💡 *Sơ đồ dưới đây được render tự động trực tiếp trên GitHub bằng Mermaid. Bạn cũng có thể tải file **`bt1.drawio`** trong repository này để mở và chỉnh sửa trực tiếp trên [Draw.io (diagrams.net)](https://app.diagrams.net).* 

```mermaid
graph TD
  Start([Bắt đầu kiểm định]) --> CheckCold{Kệ kho lạnh? is_cold_storage == 1}
  CheckCold -- Đúng --> CheckTemp{Nhiệt độ kiện hàng trong [-18.0, 5.0]?}
  CheckTemp -- Sai --> RejectTemp[Từ chối: Vi phạm quy định nhiệt độ kho lạnh] --> EndFail([Kết thúc: Từ chối nhập kho])
  CheckTemp -- Đạt --> CheckWeight
  CheckCold -- Sai --> CheckWeight{Tổng trọng lượng sau nhập <= 500.0 kg?}
  CheckWeight -- Sai --> RejectWeight[Từ chối: Vượt quá tải trọng tối đa 500kg] --> EndFail
  CheckWeight -- Đạt --> Accept([Duyệt: Đủ điều kiện nhập kho]) --> EndSuccess([Kết thúc: Thành công])
```

---

## 1. Mục tiêu và Bối cảnh bài toán

Bài tập tập trung rèn luyện kỹ năng đọc hiểu mã nguồn C legacy, phân tích luồng thực thi code (trace code) và phát hiện các lỗ hổng logic nghiệp vụ trong phân hệ quản lý kho vận WMS của Rikkei Logistics.

Hệ thống yêu cầu tuân thủ tuyệt đối 2 quy tắc cốt lõi: Quy tắc tải trọng tối đa 500.0 kg trên mỗi ô kệ và Quy tắc bảo quản kho lạnh với dải nhiệt độ nghiêm ngặt từ -18.0°C đến 5.0°C.

- Phát hiện lỗi sai sót trong toán tử điều kiện logic (logical operator bugs).
- Khắc phục triệt để lỗi bỏ sót tính toán giá trị tích lũy (cumulative values) của tải trọng kệ kho.
- Viết lại hàm kiểm định an toàn đạt chuẩn 100% quy tắc nghiệp vụ logistics.

## 2. Phân tích lỗi sai AS-IS và Báo cáo Debug

Qua quá trình đọc code và trace luồng thực thi của hàm kiem_tra_an_toan_nhap_kho hiện tại, tôi đã khoanh vùng được 2 lỗi logic nghiêm trọng dẫn đến sập kệ và hỏng hàng hóa:

Lỗi 1 (Sai logic toán tử nhiệt độ kho lạnh): Dòng code kiểm tra điều kiện 'if (item.temp_celsius < -18.0 && item.temp_celsius > 5.0)' sử dụng toán tử '&&' (Và). Điều này là bất khả thi vì không có số nào vừa nhỏ hơn -18 lại vừa lớn hơn 5 cùng một lúc. Do đó, nhánh kiểm tra kho lạnh luôn bị bỏ qua, cho phép các kiện hàng sai nhiệt độ lọt vào kho lạnh.

Lỗi 2 (Bỏ sót giá trị tích lũy tải trọng): Hàm chỉ kiểm tra 'if (item.weight_kg > MAX_SHELF_WEIGHT)' mà hoàn toàn quên cộng dồn trọng lượng hiện tại của kệ ('shelf.current_weight_kg'). Do đó, kệ đang chứa 400kg vẫn vô tư nhận thêm 150kg, tổng 550kg vượt mức 500kg quy định mà không bị chặn lại.

- Sự cố 1 (Sập kệ): Kệ 101 chứa 400kg cộng thêm 150kg pallet mới không được cộng dồn, dẫn đến tổng 550kg > 500kg phá vỡ cấu trúc kệ.
- Sự cố 2 (Hỏng thực phẩm): Hàng hóa 12.0°C lọt qua vòng kiểm tra nhiệt độ do dùng sai toán tử '&&' thay vì '||'.

## 3. Bảng phân tích Test Case thực tế

Dưới đây là bảng kiểm thử đối chiếu kết quả giữa mã nguồn cũ (AS-IS) và mã nguồn đã sửa (TO-BE):

| Test Case | Đầu vào (Input) | Kết quả SAI (AS-IS) | Kết quả ĐÚNG mong đợi (TO-BE) |
| --- | --- | --- | --- |
| TC 01 | Kệ 101 (Kho lạnh, hiện tại 400kg) + Pallet (150kg, 12°C) | [DUYỆT] (Sai hoàn toàn nghiệp vụ) | [TỪ CHỐI] Vi phạm cả quá tải và nhiệt độ kho lạnh |
| TC 02 | Kệ 102 (Kho thường, hiện tại 400kg) + Pallet (150kg, 20°C) | [DUYỆT] (Sai do không cộng dồn 400+150=550kg) | [TỪ CHỐI] Vi phạm quá tải trọng tối đa 500kg |
| TC 03 | Kệ 103 (Kho lạnh, hiện tại 200kg) + Pallet (100kg, -10°C) | [DUYỆT] (Vô tình đúng do pallet hợp lệ) | [DUYỆT] Hợp lệ mọi tiêu chuẩn lưu kho |

## 4. Source Code hoàn thiện (main.c)

Đã chỉnh sửa toàn bộ logic trong tệp mã nguồn C, sửa toán tử '&&' thành '||' cho điều kiện nằm ngoài dải nhiệt độ, đồng thời bổ sung phép tính tích lũy trọng lượng 'shelf.current_weight_kg + item.weight_kg'.

---

## 📁 Danh sách tệp tin nộp bài trong Repository
- 📝 `bt1.docx`: Báo cáo tài liệu phân tích nghiệp vụ hoàn chỉnh.
- 🎨 `bt1.drawio`: File thiết kế sơ đồ chuẩn theo quy định đề bài (mở trực tiếp bằng [Draw.io](https://app.diagrams.net) hoặc Lucidchart).
- 💻 `main.c`: Mã nguồn chương trình.
