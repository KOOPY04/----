#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <direct.h> 

using namespace std;

// 定義記錄結構
struct Record {
    string student_id;  // 學生ID
    string course_id;   // 課程ID
    string course_name; // 課程名稱
};

// 定義索引條目結構
struct IndexEntry {
    string key;  // 索引鍵值（學生ID或課程ID）
    string data; // 相關資料（課程ID或學生ID）
    long offset; // 在資料檔案中的偏移量
};

// 定義二元搜尋樹結構
struct TreeNode {
    IndexEntry entry;
    TreeNode* left;
    TreeNode* right;

    TreeNode(IndexEntry e) : entry(e), left(nullptr), right(nullptr) {}
};

// 定義課程名字索引結構
struct CourseNameIndexEntry {
    string course_id;   // 課程ID
    string course_name; // 課程名字
};

// 插入二元搜尋樹節點
TreeNode* insert(TreeNode* root, IndexEntry entry) {
    if (root == nullptr) {
        return new TreeNode(entry);
    }
    if (entry.key < root->entry.key) {
        root->left = insert(root->left, entry);
    } else {
        root->right = insert(root->right, entry);
    }
    return root;
}

// 將二元搜尋樹寫入索引文件
void writeTreeToFile(TreeNode* root, ofstream& outFile, int& fileCount, string& indexFilePathCourse) {
    if (root == nullptr) {
        return;
    }
    const int maxFileSize = 2010; // 目標文件大小限制（2KB）

    // 計算要寫入的當前節點資料的預期大小
    long long int currentSize = static_cast<long long int>(outFile.tellp()) + root->entry.key.length() + root->entry.data.length() + 2;


    // 如果超過目標文件大小，關閉當前文件，打開新文件
    if (currentSize >= maxFileSize) {
        outFile.close();
        fileCount++;
        outFile.open(indexFilePathCourse + "_" + to_string(fileCount) + ".txt");
        if (!outFile.is_open()) {
            cerr << "無法打開索引文件" << endl;
            exit(EXIT_FAILURE);
        }
        currentSize = static_cast<long long int>(outFile.tellp()) + root->entry.key.length() + root->entry.data.length() + 2;
    }

    outFile << root->entry.key << " " << root->entry.data << '\n';
    writeTreeToFile(root->left, outFile, fileCount, indexFilePathCourse);
    writeTreeToFile(root->right, outFile, fileCount, indexFilePathCourse);
    delete root; // 釋放節點
}

// 根據學生ID建立索引
void createStudentIndex(const vector<string>& dataFiles, const string& indexDir) {
    string indexFilePathStudent = indexDir + "/student_index";
    if (_mkdir(indexDir.c_str()) == -1) {
        cerr << "無法創建文件夾: " << indexDir << endl;
        exit(EXIT_FAILURE);
    }
    ofstream indexFp(indexFilePathStudent + "_1.txt");

    if (!indexFp.is_open()) {
        cerr << "無法打開索引文件" << endl;
        exit(EXIT_FAILURE);
    }

    TreeNode* root = nullptr;
    int fileCount = 1;
    

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "無法打開數據文件: " << dataFile << endl;
            continue;
        }

        string line;
        long offset = 0;
        IndexEntry entry;

        // 逐行讀取資料檔案
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // 跳過第一行

            entry.key = record.student_id;
            entry.data = record.course_id;
            entry.offset = offset;
            root = insert(root, entry);

            offset += line.length() + 1; // 更新偏移量（包含換行符）
        }

        dataFp.close();
    }

    writeTreeToFile(root, indexFp, fileCount, indexFilePathStudent);
    indexFp.close();
}

// 根據課程ID建立索引
void createCourseIndex(const vector<string>& dataFiles, const string& indexDir) {
    unordered_map<string, vector<string>> courseIndex; // 課程ID -> 學生ID列表

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

            if (record.student_id == "student_id") continue; // 跳過第一行

            courseIndex[record.course_id].push_back(record.student_id);
        }

        dataFp.close();
    }

    string indexFilePathCourse = indexDir + "/course_index";
    #ifdef _WIN32
        _mkdir(indexDir.c_str());
    #else 
        mkdir(indexDir.c_str(), 0777);
    #endif
    ofstream indexFp(indexFilePathCourse + "_1.txt");
    if (!indexFp.is_open()) {
        cerr << "無法打開索引文件" << endl;
        exit(EXIT_FAILURE);
    }

    // 建立每個課程ID的索引
    const int maxFileSize = 2010; // 目標文件大小限制（2KB）
    long long currentSize = 0;
    int fileCount = 1;

    for (const auto& entry : courseIndex) {
        string header = entry.first + "\n";
        currentSize += header.length();

        if (currentSize >= maxFileSize) {
            indexFp.close();
            fileCount++;
            indexFp.open(indexFilePathCourse + "_" + to_string(fileCount) + ".txt");
            if (!indexFp.is_open()) {
                cerr << "無法打開索引文件" << endl;
                exit(EXIT_FAILURE);
            }
            currentSize = header.length();
            indexFp << header;
        } else {
            indexFp << header;
        }

        for (const auto& studentId : entry.second) {
            string line = studentId + "\n";
            currentSize += line.length();

            if (currentSize >= maxFileSize) {
                indexFp.close();
                fileCount++;
                indexFp.open(indexFilePathCourse + "_" + to_string(fileCount) + ".txt");
                if (!indexFp.is_open()) {
                    cerr << "無法打開索引文件" << endl;
                    exit(EXIT_FAILURE);
                }
                currentSize = header.length() + line.length();
                indexFp << header;
            }

            indexFp << line;
        }
    }

    indexFp.close();
}

// 建立課程名字索引
void createCourseNameIndex(const vector<string>& dataFiles, const string& indexDir) {

    unordered_map<string, CourseNameIndexEntry> courseNameIndex; // 課程ID -> 課程名字

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "無法打開數據文件: " << dataFile << endl;
            continue;
        }

        string line;

        // 逐行讀取資料檔案
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // 跳過第一行

            // 建立課程名字索引
            CourseNameIndexEntry entry;
            entry.course_id = record.course_id;
            entry.course_name = record.course_name;
            courseNameIndex[record.course_id] = entry;
        }

        dataFp.close();
    }


    string indexFilePathCourseName = indexDir + "/courseName_index";
    if (_mkdir(indexDir.c_str()) == -1) {
        cerr << "無法創建文件夾: " << indexDir << endl;
        exit(EXIT_FAILURE);
    }
    ofstream courseNameIndexFp;
    // 將課程名字索引寫入文件
    int fileCount = 1;
    const int maxFileSize = 2010; // 目標文件大小限制（2KB）
    long long int currentSize = 0;

    courseNameIndexFp.open(indexFilePathCourseName + "_" + to_string(fileCount) + ".txt");
    if (!courseNameIndexFp.is_open()) {
        cerr << "無法打開索引文件" << endl;
        exit(EXIT_FAILURE);
    }

    // 將課程名字索引寫入文件
    for (const auto& entry : courseNameIndex) {
        // 計算要寫入的當前記錄的大小
        long long int recordSize = entry.second.course_id.length() + entry.second.course_name.length() + 2;
    
        // 如果超過目標文件大小，關閉當前文件，打開新文件
        if (currentSize + recordSize >= maxFileSize) {
            courseNameIndexFp.close();
            fileCount++;
            courseNameIndexFp.open(indexFilePathCourseName + "_" + to_string(fileCount) + ".txt");
            if (!courseNameIndexFp.is_open()) {
                cerr << "無法打開索引文件" << endl;
                exit(EXIT_FAILURE);
            }
            currentSize = 0; // 重置當前文件大小計數器
        }

        courseNameIndexFp << entry.second.course_id << " " << entry.second.course_name << '\n';
        currentSize += recordSize; // 更新當前文件大小計數器
    }

    courseNameIndexFp.close();

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
    // 讀取多個檔案
    vector<string> dataFiles = generateDataFileNames(466);

    // 建立學生ID索引
    createStudentIndex(dataFiles, "student_id_index");
    cout << "學生ID索引建立完成" << endl;

    // 建立課程ID索引
    createCourseIndex(dataFiles, "course_id_index");
    cout << "課程ID索引建立完成" << endl;

    // 建立課程名字索引
    createCourseNameIndex(dataFiles, "course_name_index");
    cout << "課程名字索引建立完成" << endl;


    return 0;
}

