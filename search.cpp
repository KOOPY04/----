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
    vector<string> course_ids;  // �ҵ{ID�C��
};

struct CourseIndexEntry {
    vector<string> student_ids;  // �ǥ�ID�C��
};

struct CourseNameIndexEntry {
    string course_id;   // �ҵ{ID
    string course_name; // �ҵ{�W��
};


// Ū���ǥͯ��ޤ��
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

// Ū���ҵ{���ޤ��
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

// Ū���ҵ{�W�r���ޤ��
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

// �ھڽҵ{�W�٬d��
string searchByCourseName(const unordered_map<string, CourseNameIndexEntry>& courseNameIndex, const string& courseID) {
    auto it = courseNameIndex.find(courseID);
    if (it == courseNameIndex.end()) {
        cout << "�䤣��ҵ{ID�� " << courseID << " ���ҵ{�W�١C" << endl;
        return "";
    }
    const CourseNameIndexEntry& entry = it->second;
    return entry.course_name;
}

// �ھھǥ�ID�d��
void searchByStudentID(const unordered_map<string, StudentIndexEntry>& studentIndex, const unordered_map<string, CourseNameIndexEntry>& courseNameIndex, const string& studentID) {
    auto it = studentIndex.find(studentID);
    if (it == studentIndex.end()) {
        cout << "�䤣��ǥ�ID�� " << studentID << " ������ҵ{�C" << endl;
        return;
    }

    ofstream outFile("student_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "�L�k���}���G���: student_query_result.txt" << endl;
        return;
    }

    const StudentIndexEntry& entry = it->second;

    outFile << "�ǥ�ID: " << studentID << '\n';

    int countCourse = 0;

    for (const auto& course_id : entry.course_ids) {
        string course_name = searchByCourseName(courseNameIndex, course_id);
        outFile << "�ҵ{ID: " << course_id << ", �ҵ{�W��: " << course_name << '\n';
        countCourse++;
    }

    outFile << "�`�@ " << countCourse << " ���ҵ{�C" << endl;
    outFile.close();
    cout << "�d�ߵ��G�w�g�J student_query_result.txt" << endl;
}

// �ھڽҵ{ID�d��
void searchByCourseID(const unordered_map<string, CourseIndexEntry>& courseIndex, const unordered_map<string, CourseNameIndexEntry>& courseNameIndex, const string& courseID) {
    auto it = courseIndex.find(courseID);
    cout << it->first << endl;
    if (it == courseIndex.end()) {
        cout << "�䤣��ҵ{ID�� " << courseID << " ������ǥ͡C" << endl;
        return;
    }

    ofstream outFile("course_query_result.txt");
    if (!outFile.is_open()) {
        cerr << "�L�k���}���G���: course_query_result.txt" << endl;
        return;
    }

    const CourseIndexEntry& entry = it->second;

    outFile << "�ҵ{ID: " << courseID << '\n';
    string course_name = searchByCourseName(courseNameIndex, courseID);
    outFile << "�ҵ{�W��: " << course_name << '\n';

    outFile << "��Ū�ǥ�ID�C��:\n";

    int countStudent = 0;

    for (const auto& student_id : entry.student_ids) {
        outFile << student_id << '\n';
        countStudent++;
    }

    outFile << "�`�@ " << countStudent << " ��ǥ͡C" << endl;
    outFile.close();
    cout << "�d�ߵ��G�w�g�J course_query_result.txt" << endl;
}


int main() {

    const string studentIndexFile = "student_index";
    const string courseIndexFile = "course_index";
    const string courseNameIndexFile = "courseName_index";

    // Ū���ǥ�ID���ޤ��
    unordered_map<string, StudentIndexEntry> studentIndex = readStudentIndex(studentIndexFile);

    // Ū���ҵ{ID���ޤ��
    unordered_map<string, CourseIndexEntry> courseIndex = readCourseIndex(courseIndexFile);

    // Ū���ҵ{�W�ٯ��ޤ��
    unordered_map<string, CourseNameIndexEntry> courseNameIndex = readCourseNameIndex(courseNameIndexFile);

    char choice;
    cout << "�п�ܭn�d�ߪ����� ('I' ���ǥ�ID, 'C' ���ҵ{ID): ";
    cin >> choice;

    if (choice == 'I') {
        string studentID;
        cout << "�п�J�n�d�ߪ��ǥ�ID (�Ҧp: D1149488): ";
        cin >> studentID;
    
        searchByStudentID(studentIndex, courseNameIndex, studentID);
    } else if (choice == 'C') {
        string courseID;
        cout << "�п�J�n�d�ߪ��ҵ{ID (�Ҧp: 2142): ";
        cin >> courseID;

        searchByCourseID(courseIndex, courseNameIndex, courseID);
    } else {
        cout << "�L�Ī���ܡA�п�J 'I', 'C'\n";
    }

    return 0;
}
