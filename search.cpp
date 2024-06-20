#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>
#include <direct.h>
#include <algorithm>


using namespace std;


struct StudentIndexEntry {
    vector<string> course_ids;  // 課程ID列表
};

struct CourseIndexEntry {
    vector<string> student_ids;  // 學生ID列表
};

struct CourseNameIndexEntry {
    string course_id;   // 課程ID
    string course_name; // 課程名稱
};


// 讀取學生索引文件
unordered_map<string, StudentIndexEntry> readStudentIndex(const string& baseFilename) {

    unordered_map<string, StudentIndexEntry> index;
    int i = 1;

    while(true){
        string filename = "student_id_index/" + baseFilename + "_" + to_string(i) + ".txt";
        ifstream infile(filename);

        if (!infile) {
            break;
        }
        
        string studentId;
        string courseId;
        while (infile >> studentId >> courseId) {
            index[studentId].course_ids.push_back(courseId);
        }

        infile.close();
        i++;
    }
    return index;
}

// 讀取課程索引文件
unordered_map<string, CourseIndexEntry> readCourseIndex(const string& baseFilename) {
    unordered_map<string, CourseIndexEntry> index;
    int i = 1;

    while (true) {
        string filename = "course_id_index/" + baseFilename + "_" + to_string(i) + ".txt";
        ifstream infile(filename);

        if (!infile) {
            break;
        }

        string line;
        string courseId;
        while (getline(infile, line)) {
            if (line.length() == 4) {
                courseId = line;
            } else {
                index[courseId].student_ids.push_back(line);
            }
        }
        infile.close();
        i++;
    }
    return index;
}

// 讀取課程名字索引文件
unordered_map<string, CourseNameIndexEntry> readCourseNameIndex(const string& baseFilename) {
    unordered_map<string, CourseNameIndexEntry> index;
    int i = 1;
    
    while(true){
        string filename = "course_name_index/" + baseFilename + "_" + to_string(i) + ".txt";

        ifstream infile(filename);
        if (!infile) {
            break;
        }

        string line;
        while (getline(infile, line)) {
            // cout << line << endl;
            stringstream ss(line);
            string courseId;
            string courseName;
            ss >> courseId >> courseName;
            index[courseId] = {courseId, courseName};
        }

        infile.close();
        i++;
    }
    return index;
}

// 根據課程名稱查找
string searchByCourseName(const unordered_map<string, CourseNameIndexEntry>& courseNameIndex, const string& courseID) {
    auto it = courseNameIndex.find(courseID);
    if (it == courseNameIndex.end()) {
        cout << "找不到課程ID為 " << courseID << " 的課程名稱。" << endl;
        return "";
    }
    const CourseNameIndexEntry& entry = it->second;
    return entry.course_name;
}

// 根據學生ID查找
void searchByStudentID(const unordered_map<string, StudentIndexEntry>& studentIndex, const unordered_map<string, CourseNameIndexEntry>& courseNameIndex, const string& studentID) {
    auto it = studentIndex.find(studentID);
    if (it == studentIndex.end()) {
        cout << "找不到學生ID為 " << studentID << " 的任何課程。" << endl;
        return;
    }

    ofstream outFile("student_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "無法打開結果文件: student_query_result.txt" << endl;
        return;
    }

    const StudentIndexEntry& entry = it->second;

    outFile << "學生ID: " << studentID << '\n';

    int countCourse = 0;

    for (const auto& course_id : entry.course_ids) {
        string course_name = searchByCourseName(courseNameIndex, course_id);
        outFile << "課程ID: " << course_id << ", 課程名稱: " << course_name << '\n';
        countCourse++;
    }

    outFile << "總共 " << countCourse << " 門課程。" << endl;
    outFile.close();
    cout << "查詢結果已寫入 student_query_result.txt" << endl;
}

// 根據課程ID查找
void searchByCourseID(const unordered_map<string, CourseIndexEntry>& courseIndex, const unordered_map<string, CourseNameIndexEntry>& courseNameIndex, const string& courseID) {
    auto it = courseIndex.find(courseID);
    cout << it->first << endl;
    if (it == courseIndex.end()) {
        cout << "找不到課程ID為 " << courseID << " 的任何學生。" << endl;
        return;
    }

    ofstream outFile("course_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "無法打開結果文件: course_query_result.txt" << endl;
        return;
    }

    const CourseIndexEntry& entry = it->second;

    outFile << "課程ID: " << courseID << '\n';
    string course_name = searchByCourseName(courseNameIndex, courseID);
    outFile << "課程名稱: " << course_name << '\n';

    outFile << "修讀學生ID列表:\n";

    int countStudent = 0;

    for (const auto& student_id : entry.student_ids) {
        outFile << student_id << '\n';
        countStudent++;
    }

    outFile << "總共 " << countStudent << " 位學生。" << endl;
    outFile.close();
    cout << "查詢結果已寫入 course_query_result.txt" << endl;
}


int main() {

    const string studentIndexFile = "student_index";
    const string courseIndexFile = "course_index";
    const string courseNameIndexFile = "courseName_index";

    // 讀取學生ID索引文件
    unordered_map<string, StudentIndexEntry> studentIndex = readStudentIndex(studentIndexFile);

    // 讀取課程ID索引文件
    unordered_map<string, CourseIndexEntry> courseIndex = readCourseIndex(courseIndexFile);

    // 讀取課程名稱索引文件
    unordered_map<string, CourseNameIndexEntry> courseNameIndex = readCourseNameIndex(courseNameIndexFile);

    char choice;
    cout << "請選擇要查詢的類型 ('I' 為學生ID, 'C' 為課程ID): ";
    cin >> choice;

    if (choice == 'I') {
        string studentID;
        cout << "請輸入要查詢的學生ID (例如: D1149488): ";
        cin >> studentID;
    
        searchByStudentID(studentIndex, courseNameIndex, studentID);
    } else if (choice == 'C') {
        string courseID;
        cout << "請輸入要查詢的課程ID (例如: 2142): ";
        cin >> courseID;

        searchByCourseID(courseIndex, courseNameIndex, courseID);
    } else {
        cout << "無效的選擇，請輸入 'I', 'C'\n";
    }

    return 0;
}
