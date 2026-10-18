#include <stdio.h>
#define MAX_SHELF_WEIGHT 500.0

typedef struct {
    int pallet_id;
    float weight_kg;
    float temp_celsius;
} PalletItem;

typedef struct {
    int shelf_id;
    int is_cold_storage;
    float current_weight_kg;
} WarehouseShelf;

int kiem_tra_an_toan_nhap_kho(WarehouseShelf shelf, PalletItem item) {
    if (shelf.is_cold_storage == 1) {
        if (item.temp_celsius < -18.0 || item.temp_celsius > 5.0) {
            printf('[LỖI KHO LẠNH] Nhiệt độ bảo quản %.1f°C không đạt chuẩn (-18°C đến 5°C)!\n', item.temp_celsius);
            return 0;
        }
    }
    float tong_trong_luong = shelf.current_weight_kg + item.weight_kg;
    if (tong_trong_luong > MAX_SHELF_WEIGHT) {
        printf('[LỖI QUÁ TẢI] Tổng trọng lượng sau khi xếp là %.1f kg vượt giới hạn 500kg của kệ kho!\n', tong_trong_luong);
        return 0;
    }
    return 1;
}

int main() {
    WarehouseShelf ke_lanh_101 = {101, 1, 400.0};
    PalletItem pallet_moi = {5001, 150.0, 12.0};

    printf('=== HỆ THỐNG GIÁM SÁT AN TOÀN NHẬP KHO LOGISTICS ===\n');
    printf('Thông tin kệ 101 : Tải trọng hiện tại = %.1f kg | Loại kho = %s\n', 
           ke_lanh_101.current_weight_kg, 
           ke_lanh_101.is_cold_storage ? 'Kho Lạnh' : 'Kho Thường');
    printf('Kiện hàng nhập   : Mã = %d | Nặng = %.1f kg | Nhiệt độ = %.1f°C\n',
           pallet_moi.pallet_id, pallet_moi.weight_kg, pallet_moi.temp_celsius);
    printf('----------------------------------------------------\n');

    int ket_qua = kiem_tra_an_toan_nhap_kho(ke_lanh_101, pallet_moi);

    if (ket_qua == 1) {
        printf('=> KẾT QUẢ: [DUYỆT] Đủ điều kiện nhập kho!\n');
    } else {
        printf('=> KẾT QUẢ: [TỪ CHỐI] Vi phạm quy định an toàn lưu kho!\n');
    }

    return 0;
}