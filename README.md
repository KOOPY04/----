以附檔資料檔為基礎實作 index 機制，專題需求如下

(1) 為 course_id, student_id 建立 index 以利查詢，index 需存於檔案中，每個 index 檔案中僅能包含查詢的 key 以及相關資料檔或 index 檔名 (reference)，index 檔案大小限制為 2KB；

(2) 兩項功能，輸入 ID 查找該 ID  所選的全部課程、輸入課號查找該課號全部的選課學生 (輸出 ID 應排序並統計人數以利驗證)；

(3) 以 C/C++ 進行實作，同時間僅譨讀取一個 index 檔/資料檔，但是記憶體的暫存則沒有限制。


course_id index 以 hash 實作

student_id index 二元搜尋樹 實作

搜尋時可以顯示課程中文名稱

為課程中文名稱建立 index 
