#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream>

using namespace std;

struct Record {
    string student_id;  // 學生ID
    string course_id;   // 課程ID
    string course_name; // 課程名稱
};

// 讀取學生索引文件
unordered_map<string, long> readStudentIndex(const string& filename) {
    unordered_map<string, long> index;
    ifstream infile(filename);
    if (!infile) {
        cerr << "無法打開學生索引文件: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    string studentId;
    long offset;
    while (infile >> studentId >> offset) {
        index[studentId] = offset;
    }

    infile.close();
    return index;
}

// 讀取課程索引文件
unordered_map<string, long> readCourseIndex(const string& filename) {
    unordered_map<string, long> index;
    ifstream infile(filename);
    if (!infile) {
        cerr << "無法打開課程索引文件: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    string courseId;
    long offset;
    while (infile >> courseId >> offset) {
        index[courseId] = offset;
    }

    infile.close();
    return index;
}

// 根據學生ID查詢
void searchByStudentID(const unordered_map<string, long>& studentIndex, const vector<string>& dataFiles, const string& studentID) {
    auto it = studentIndex.find(studentID);
    if (it == studentIndex.end()) {
        cout << "找不到學生ID " << studentID << " 的任何課程。" << endl;
        return;
    }

    long offset = it->second;
    int countCourse = 0;
    ofstream outFile("student_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "無法打開結果文件: student_query_result.txt" << endl;
        return;
    }

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "無法打開數據文件: " << dataFile << endl;
            continue;
        }

        string line;
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == studentID) {
                outFile << "課程ID: " << record.course_id << ", 課程名稱: " << record.course_name << '\n';
                countCourse++;
            }

            offset += line.length() + 1; // 更新偏移量（包含換行符）
        }

        dataFp.close();
    }
    
    outFile << "學生ID " << studentID << " 共修讀了 " << countCourse << " 門課程" << endl;
    outFile.close();
    cout << "查詢結果已寫入 student_query_result.txt" << endl;
}


// 根據課程ID查詢
void searchByCourseID(const unordered_map<string, long>& courseIndex, const vector<string>& dataFiles, const string& courseID) {
    auto it = courseIndex.find(courseID);
    if (it == courseIndex.end()) {
        cout << "找不到課程ID " << courseID << " 的任何學生。" << endl;
        return;
    }

    long offset = it->second;
    int countStudent = 0;
    string courseName; // 用於存儲課程名稱
    bool foundCourseName = false; // 標記是否已找到課程名稱

    ofstream outFile("course_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "無法打開結果文件: course_query_result.txt" << endl;
        return;
    }

    // 遍歷所有數據文件
    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "無法打開數據文件: " << dataFile << endl;
            continue;
        }

        string line;
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.course_id == courseID) {
                if (!foundCourseName) {
                    courseName = record.course_name; // 獲取課程名稱
                    foundCourseName = true;
                    // 在文件開頭寫入課程ID和課程名稱
                    outFile << "課程ID: " << courseID << ", 課程名稱: " << courseName << '\n';
                }
                outFile << "學生ID: " << record.student_id << '\n';
                countStudent++;
            }

            offset += line.length() + 1; // 更新偏移量（包含換行符）
        }

        dataFp.close();
    }

    if (!foundCourseName) {
        cout << "找不到課程ID " << courseID << " 的課程名稱。" << endl;
    } else {
        outFile << "課程ID " << courseID << " 共有 " << countStudent << " 位學生" << endl;
    }

    outFile.close();
    cout << "查詢結果已寫入 course_query_result.txt" << endl;
}


// 構造數據文件名稱列表
vector<string> generateDataFileNames(int fileCount) {
    vector<string> dataFiles;
    for (int i = 1; i <= fileCount; i++) {
        stringstream fileNo;
        if (i < 10) {
            fileNo << "data_big5/000" << i;
        } else if (i < 100) {
            fileNo << "data_big5/00" << i;
        } else {
            fileNo << "data_big5/0" << i;
        }
        dataFiles.push_back(fileNo.str());
    }
    return dataFiles;
}

int main() {
    const string studentIndexFile = "student_index.txt"; // 學生ID索引文件名
    const string courseIndexFile = "course_index.txt";   // 課程ID索引文件名

    // 讀取學生索引文件
    unordered_map<string, long> studentIndex = readStudentIndex(studentIndexFile);

    // 讀取課程索引文件
    unordered_map<string, long> courseIndex = readCourseIndex(courseIndexFile);

    // 假設您有一個函數可以生成文件名列表
    vector<string> dataFiles = generateDataFileNames(466); // 假設這裡已經生成了數據文件名列表

    char choice;
    cout << "請選擇要查詢的類型 ('I' 為學生ID, 'C' 為課程ID): ";
    cin >> choice;

    if (choice == 'I') {
        string studentID;
        cout << "請輸入要查詢的學生ID (例如: D1149488): ";
        cin >> studentID;

        searchByStudentID(studentIndex, dataFiles, studentID);
    } else if (choice == 'C') {
        string courseID;
        cout << "請輸入要查詢的課程ID (例如: 2142): ";
        cin >> courseID;

        searchByCourseID(courseIndex, dataFiles, courseID);
    } else {
        cout << "無效的選擇，請輸入 'I' 或 'C'\n";
    }

    return 0;
}

