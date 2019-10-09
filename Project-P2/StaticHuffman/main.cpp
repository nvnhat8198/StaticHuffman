#include <stdio.h>
#include<string.h>
#include<conio.h>
#include<iostream>
#include<Windows.h>
#include <filesystem>	// Thư viện hỗ trợ lấy tên file trong thư mục
using namespace std::experimental::filesystem;	
using namespace std;

#define MAX 1024
const int MAX_NODE = 511;
const int MAX_BIT_LEN = 10000;

struct HUFFNODE {
	unsigned char c;		// ky tu
	int		freq;			// so lan xuat hien
	bool	used;			// danh dau node da xu ly chua 
	int		nLeft;			// chi so nut con nam ben trai
	int		nRight;			// chi so nut con nam ben phai
};

struct MABIT {
	char*	bits;
	int		soBit;
};


HUFFNODE	huffTree[MAX_NODE];
MABIT	bangMaBit[256];
int posRoot;
HUFFNODE ROOT;


struct Node {
	char* data;
	Node*next;
};
Node*getNode(char* x) {
	Node*p = new Node;
	if (p == NULL)return NULL;
	p->data = x;
	p->next = NULL;
	return p;
}
void AddTail(Node*&head, Node*&tail, char* x) {
	Node*p = getNode(x);
	if (head == NULL)head = tail = p;
	else {
		tail->next = p;
		tail = p;
	}
}
void LayTenFile(Node*&head, Node*&tail, char*DuongDan) {
	std::experimental::filesystem::path p(DuongDan);
	for (auto i = directory_iterator(p); i != directory_iterator(); i++)
	{
		if (!is_directory(i->path()))
		{
			string x;
			x = i->path().filename().string();
			char *tmp = new char[x.length() + 1];
			tmp = std::strcpy(tmp, x.c_str());		// chuyển string sang biến char*
			AddTail(head, tail, tmp);		// lấy tên file đưa vào cuối dslk đơn
		}
		else
			continue;
	}
}
void HuyLinkList(Node*&head, Node*&tail) {
	while (head) {
		Node*p = head;
		head = head->next;
		p->next = NULL;
		delete p;
	}
	head = tail = NULL;
}
int SoLuongFile(Node*head, Node*tail) {
	int count = 0;
	for (Node*p = head; p; p = p->next)count++;
	return count;
}
char*TenFileCanNen(Node*head, Node*tail, int ThuTu) {		// Lấy ra tên file cần xử lí lưu trong DSLK dựa vào thứ tự lấy
	Node*p = head;
	int dem = 0;
	while (dem < ThuTu) {
		dem++;
		p = p->next;
	}
	return p->data;
}

void KhoiTao() {
	for (int i = 0; i < MAX_NODE; i++) {
		huffTree[i].c = i;
		huffTree[i].freq = 0;
		huffTree[i].used = false;
		huffTree[i].nLeft = -1;
		huffTree[i].nRight = -1;
	}
}

void ThongKeTanSo(char* tenFile) {
	FILE* fi = fopen(tenFile, "rt");
	unsigned char c;
	while (1) {
		fscanf(fi, "%c", &c);
		if (feof(fi)) break;
		huffTree[c].freq++;
	}
	fclose(fi);
}

//void XuatBangThongKe() {
//	printf("Bang thong ke tan suat: \n");
//	for (int i = 0; i < 256; i++) {
//		if (huffTree[i].freq > 0) {
//			printf("%c: %d\n", i, huffTree[i].freq);
//		}
//	}
//}

bool Tim2PhanTuNhoNhat(int &i, int &j, int nNode) {
	i = -1; j = -1;
	int vitridau = 0;

	while (vitridau<nNode) {
		if (huffTree[vitridau].freq > 0 && huffTree[vitridau].used == false) break;
		vitridau++;
	}
	if (vitridau == nNode)return false;
	i = vitridau;

	// Tìm i là vị trí nhỏ nhất
	for (int k = vitridau; k < nNode; k++) {
		if (huffTree[k].freq > 0 && huffTree[k].used == false) {
			if (huffTree[k].freq < huffTree[i].freq)i = k;
			else if (huffTree[k].freq == huffTree[i].freq&&huffTree[k].c < huffTree[i].c)i = k;
		}
	}

	// Tìm j là vị trí nhỏ nhất nhưng lớn hơn i
	while (vitridau<nNode) {
		if (huffTree[vitridau].freq > 0 && huffTree[vitridau].used == false)
		{
			if (vitridau != i)break;
		}
		vitridau++;
	}
	if (vitridau == nNode)return false;
	j = vitridau;
	for (int k = vitridau; k < nNode; k++) {
		if (huffTree[k].freq > 0 && huffTree[k].used == false) {
			if (k != i) {
				if (huffTree[k].freq < huffTree[j].freq)j = k;
				else if (huffTree[k].freq == huffTree[j].freq&&huffTree[k].c < huffTree[j].c)j = k;
			}
		}
	}
	if (i == -1 || j == -1)return false;
	return true;
}

int TaoCayHuffman() {
	int nNode = 256;
	int i, j;
	bool timThay = false;
	while (true) {
		timThay = Tim2PhanTuNhoNhat(i, j, nNode);
		if (!timThay) {
			break;
		}

		huffTree[nNode].c = huffTree[i].c;
		huffTree[nNode].freq = huffTree[i].freq + huffTree[j].freq;
		huffTree[nNode].nLeft = i;
		huffTree[nNode].nRight = j;

		huffTree[i].used = true;
		huffTree[j].used = true;

		huffTree[nNode].used = false;
		nNode++;
	}
	return nNode - 1; // trả về vị trí node gốc

}

void DuyetCayHuffman(int node, char maBit[], int nMaBit) {
	if (node == -1) {
		return;
	}
	if (huffTree[node].nLeft == -1 && huffTree[node].nRight == -1) {
		bangMaBit[node].soBit = nMaBit;
		bangMaBit[node].bits = new char[nMaBit];
		for (int i = 0; i < nMaBit; i++) {
			bangMaBit[node].bits[i] = maBit[i];
		}
		return;
	}
	else {
		maBit[nMaBit] = 0;
		DuyetCayHuffman(huffTree[node].nLeft, maBit, nMaBit + 1);
		maBit[nMaBit] = 1;
		DuyetCayHuffman(huffTree[node].nRight, maBit, nMaBit + 1);
	}
}

void PhatSinhMaBit(int nRoot) {
	for (int i = 0; i < 256; i++) {
		bangMaBit[i].soBit = 0;
		bangMaBit[i].bits = NULL;
	};
	char maBit[MAX_BIT_LEN / 8];
	int nMaBit = 0;
	DuyetCayHuffman(nRoot, maBit, nMaBit);
}

//void XuatBangMaBit() {
//	for (int i = 0; i < 256; i++)
//		if (bangMaBit[i].soBit > 0) {
//			printf("%c: ", i);
//			for (int j = 0; j < bangMaBit[i].soBit; j++)
//				printf("%d", bangMaBit[i].bits[j]);
//			printf("\n");
//		}
//}

void MaHoa1KyTu(unsigned char c, unsigned char &out, unsigned char &viTriBit, FILE*f, int &count) {

	for (int i = 0; i < bangMaBit[c].soBit; i++) {
		if (bangMaBit[c].bits[i] == 1) {
			out = out | (1 << viTriBit); // bật bit
		}
		if (viTriBit == 0) {		// đủ 1 byte ghi ra file
			viTriBit = 7;
			putc(out, f);
			out = 0;
			count++;
		}
		else viTriBit--;
	}
}

char* NoiDuongDan(char*DuongDan, char*tenfile) {
	char*kq;
	int len = strlen(DuongDan) + strlen(tenfile) + 3;
	kq = new char[len];
	kq = strcpy(kq, DuongDan);
	kq = strcat(kq, "\\");
	kq = strcat(kq, tenfile);
	kq[len] = '\0';
	return kq;
}

void EnCode(char*DuongDan, char* inputFile, FILE*Encode, FILE*Header, int& ViTriBatDau, int &STT, int DoDaiTenFile) {
	char * FileNen;
	FileNen = NoiDuongDan(DuongDan, inputFile);
	FILE *fi = fopen(FileNen, "rb");

	fseek(fi, 0, SEEK_END);
	int SoByteTruocKhiNen = ftell(fi);
	fseek(fi, 0, SEEK_SET);

	// thống kê tần số xuất hiện của các kí tự trong file
	KhoiTao();
	ThongKeTanSo(FileNen);
//	XuatBangThongKe();

	// Tạo cây Huffman
	posRoot = TaoCayHuffman();
	ROOT = huffTree[posRoot];

	// phát sinh mã bit
	PhatSinhMaBit(posRoot);
//	XuatBangMaBit();

	// thay thế các kí tự bằng mã bit tương ứng
	unsigned char out = 0;				// kí tự được xuất ra file
	unsigned char soBitCoNghia = 0;		// số bit có nghĩa của byte cuối cùng

	unsigned char c;
	unsigned char viTriBit = 7;

	int soByte = 0;			// Số byte sau nén
	while (true) {
		fscanf(fi, "%c", &c);
		if (feof(fi))break; 
		MaHoa1KyTu(c, out, viTriBit, Encode, soByte);
	}

	soBitCoNghia = 7 - viTriBit;
	if (soBitCoNghia == 0) {
		soBitCoNghia = 8;
	}
	else {
		putc(out, Encode);
		soByte++;
	}
			// Ghi ra File Header theo thứ tự:
			// stt, độ dài tên file, tên file, vị trí bắt đầu, số byte trước khi nén,số byte, số bit có nghĩa của byte cuối
	fprintf(Header, "%d %d %s %d %d %d %d\n", STT, DoDaiTenFile, inputFile, ViTriBatDau, SoByteTruocKhiNen, soByte, soBitCoNghia);
	ViTriBatDau += soByte;
	STT++;
	fclose(fi);
}

void DeCode (char *DuongDanFolderOutput, int ThuTuFileGiaiNen)
{
	unsigned char c;
	int STT, DoDaiTenFile;
	char DuongDanFileout[MAX],TenFileout[MAX], DuongDanFolderInput[MAX], tenFile[MAX];
	int ViTriBatDau, soByteBanDau, soByteDaNen, soBitCoNghiaCuaByteCuoi;
	int dem = 0, SoLuongFile, kichthuocfilesaunen = 0;
	FILE*Header = fopen("Header.txt", "rt");
	fscanf(Header, "%d", &SoLuongFile);
	fscanf(Header, "%s", &DuongDanFileout);
	fscanf(Header, "%s", &TenFileout);
	fscanf(Header, "%s", &DuongDanFolderInput);

	char *FileEncode = NoiDuongDan(DuongDanFileout, TenFileout);
	FILE*Encode = fopen(FileEncode, "rb");

	if (SoLuongFile < ThuTuFileGiaiNen || ThuTuFileGiaiNen <= 0) {
		printf("Giai nen khong duoc\n");
		return;
	}
	while (dem < ThuTuFileGiaiNen){
		dem++;
		fscanf(Header, "%d", &STT);
		fscanf(Header, "%d", &DoDaiTenFile);
		fseek(Header, 1, SEEK_CUR);
		fgets(tenFile, DoDaiTenFile+1, Header);
		fscanf(Header, "%d", &ViTriBatDau);
		fseek(Encode, ViTriBatDau, SEEK_SET);
		fscanf(Header, "%d", &soByteBanDau);
		fscanf(Header, "%d", &soByteDaNen);
		fscanf(Header, "%d", &soBitCoNghiaCuaByteCuoi);
	}

	char*FileGiaiNen;
	FileGiaiNen = NoiDuongDan(DuongDanFolderOutput, tenFile);
	FILE*Decode = fopen(FileGiaiNen, "w");

	char * FileNen;
	FileNen = NoiDuongDan(DuongDanFolderInput, tenFile);

	// Tạo lại cây Huffman dựa vào bảng thống kê tần số
	KhoiTao();
	ThongKeTanSo(FileNen);
	posRoot = TaoCayHuffman();
	ROOT = huffTree[posRoot];


	int lenght = (soByteDaNen - 1) * 8 + soBitCoNghiaCuaByteCuoi + 1;
	int index = 0;
	char *tmp = new char[lenght];		// Mảng tạm lưu chuỗi mã bit

	for (int i = 0; i < soByteDaNen - 1; i++) {
		c = (unsigned char)getc(Encode);
		for (int j = 7; j >= 0; j--) {
			if ((c >> j) & 1)tmp[index] = '1';		// lấy bit
			else tmp[index] = '0';
			index++;
		}
	}
	c = (unsigned char)getc(Encode);
	for (int i = 7; i >= 7 - soBitCoNghiaCuaByteCuoi + 1; i--) {
		if ((c >> i) & 1)tmp[index] = '1';			// lấy bit
		else tmp[index] = '0';
		index++;
	}

	tmp[index] = '\0';

	int posTmp = posRoot;
	for (int i = 0; i < index; i++) {
		if (tmp[i] == 48) {				// Bit 0
			if (ROOT.nLeft == -1 && ROOT.nRight == -1) {
				if (ROOT.c == '\n')kichthuocfilesaunen += 2;// '\n' tương đương vs kí tự xuống dòng (13) và sang dòng mới (10) nên tăng 2 byte
				else kichthuocfilesaunen++;		// tăng 1 byte nếu là 1 kí tự khác '\n'
				fputc(ROOT.c, Decode);
				posTmp = posRoot;
				ROOT = huffTree[posTmp];
				i--;
			}
			else {
				posTmp = huffTree[posTmp].nLeft;
				ROOT = huffTree[posTmp];
			}
		}
		else if (tmp[i] == 49) {		// Bit 1
			if (ROOT.nLeft == -1 && ROOT.nRight == -1) {
				if (ROOT.c == '\n')kichthuocfilesaunen += 2;
				else kichthuocfilesaunen++;
				fputc(ROOT.c, Decode);
				posTmp = posRoot;
				ROOT = huffTree[posTmp];
				i--;
			}
			else {
				posTmp = huffTree[posTmp].nRight;
				ROOT = huffTree[posTmp];
			}
		}
	}
	if (ROOT.c == '\n')kichthuocfilesaunen += 2;
	else kichthuocfilesaunen++;	
	fputc(ROOT.c, Decode);
	ROOT = huffTree[posRoot];

	// kIỂM TRA CHECKSUM SAU KHI GIẢI NÉN
	Sleep(250);
	if (kichthuocfilesaunen == soByteBanDau)printf(" -> Kiem tra checksum sau giai nen file %s => khong co loi.\n", tenFile);
	else printf(" -> Kiem tra checksum sau giai nen file %s => co loi.\n", tenFile);
	delete[]tmp;
	tmp = NULL;

	fclose(Decode);
	fclose(Header);
	fclose(Encode);
}


void main() {
	int ViTriBatDau = 0, STT = 1;
	Node*head, *tail;
	head = tail = NULL;
	char Folder_In[MAX], Folder_Out[MAX], FileEncode[MAX], File_Out[MAX];

	printf("\n\t    -> MENU <-\n\n");
	printf(" 1. Nen tat ca cac file trong folder\n 2. Xem noi dung file da nen (phan Header)\n 3. Giai nen tat ca cac file trong tap tin nen\n 4. Chon va giai nen cac file rieng le\n\n");
	printf(" => Lua chon cua ban: ");
	int chon;
	scanf("%d", &chon);
	printf("\n");
	if (chon == 1) {
		// Menu 1
		printf("Nhap duong dan toi Folder chua cac File can nen (folder_in): ");
		scanf("%s", &Folder_In);	// VD: E:\FILEINPUT
		LayTenFile(head, tail, Folder_In);
		int soluongfile = SoLuongFile(head, tail);
		printf("Nhap duong dan de luu File nen (file_out): ");
		scanf("%s", &File_Out);
		printf("Nhap ten File nen: ");
		scanf("%s", &FileEncode);
		char*TenFileEncode = NoiDuongDan(File_Out, FileEncode);
		FILE*Encode; FILE*Header;
		Encode = fopen(TenFileEncode, "wb");
		Header = fopen("Header.txt", "wt");
		fprintf(Header, "%d %s %s %s\n", soluongfile, File_Out, FileEncode, Folder_In);
		for (int i = 0; i < soluongfile; i++) {
			char*FileDuocNen = TenFileCanNen(head, tail, i);
			int n = strlen(FileDuocNen);
			EnCode(Folder_In, FileDuocNen, Encode, Header, ViTriBatDau, STT,n);
		}
		printf("\n => Da nen thanh cong!");
		fclose(Encode);
		fclose(Header);
	}
	else if (chon == 2) {
		// Menu 2
		FILE*f = fopen("Header.txt", "rt");
		if (f == NULL)printf(" -> Ban chua nen File truoc do!");
		else {
			int soFile, stt, vitridau, kichthuoctruoc, kichthuocsau, sobitconghia,DoDaiTenFile;
			char DuongDanFileout[MAX], TenFileout[MAX], DuongDanFolderInput[MAX], tenfile[MAX];
			fscanf(f, "%d", &soFile);
			fscanf(f, "%s", &DuongDanFileout);
			fscanf(f, "%s", &TenFileout);
			fscanf(f, "%s", &DuongDanFolderInput);
			printf("\n Co %d File da duoc nen.\n\n STT\t\t\tTen File\t\t\tSize truoc khi nen\t\t\tSize sau khi nen\n\n", soFile);
			for (int i = 0; i < soFile; i++) {
				fscanf(f, "%d", &stt);
				fscanf(f, "%d", &DoDaiTenFile);
				fseek(f, 1, SEEK_CUR);
				fgets(tenfile, DoDaiTenFile+1, f);
				fscanf(f, "%d", &vitridau);
				fscanf(f, "%d", &kichthuoctruoc);
				fscanf(f, "%d", &kichthuocsau);
				fscanf(f, "%d", &sobitconghia);
				Sleep(250);
				printf(" %d\t\t\t%s\t\t\t%d(bytes)\t\t\t\t%d(bytes)\n", stt, tenfile, kichthuoctruoc, kichthuocsau);
			}
			fclose(f);
		}
	}
	else if (chon == 3) {
		// Menu 3
		FILE*f = fopen("Header.txt", "rt");
		if (f == NULL)printf(" -> Ban chua nen File truoc do!");
		else {
			printf("Nhap duong dan toi Folder chua cac File se duoc giai nen (folder_out): ");
			scanf("%s", &Folder_Out);	// VD: E:\FILEOUTPUT
			int soFile;
			char DuongDanFileout[MAX], TenFileout[MAX], DuongDanFolderInput[MAX];
			fscanf(f, "%d", &soFile);
			fscanf(f, "%s", &DuongDanFileout);
			fscanf(f, "%s", &TenFileout);
			fscanf(f, "%s", &DuongDanFolderInput);
			fclose(f);
			for (int i = 1; i <= soFile; i++) DeCode(Folder_Out, i);
			printf("\n => Da giai nen!");
		}
	}
	else if (chon == 4) {
		// Menu 4
		FILE*f = fopen("Header.txt", "rt");
		if(f==NULL) printf(" -> Ban chua nen File truoc do!");
		else {
			printf("Nhap duong dan toi Folder chua cac File se duoc giai nen (folder_out): ");			
			scanf("%s", &Folder_Out);	// VD: E:\FILEOUTPUT
			int soFile;
			char DuongDanFileout[MAX], TenFileout[MAX], DuongDanFolderInput[MAX];
			fscanf(f, "%d", &soFile);
			fscanf(f, "%s", &DuongDanFileout);
			fscanf(f, "%s", &TenFileout);
			fscanf(f, "%s", &DuongDanFolderInput);
			fclose(f);
			int sl;
			printf("Co %d File da duoc nen, nhap so luong File muon giai nen: ", soFile);
			scanf("%d", &sl);
			while (sl <= 0 || sl > soFile) {
				printf("Nhap khong hop le, nhap lai so luong File muon giai nen: ");
				scanf("%d", &sl);
			}
			int tt;
			for (int i = 0; i < sl; i++) {
				printf("Nhap thu tu File can giai nen thu %d: ", i + 1);
				scanf("%d", &tt);
				DeCode(Folder_Out, tt);
			}
			printf("\n => Da giai nen!");
		}
	}
	else printf("\nBan chon khong hop le, chuong tring ket thuc.");

	HuyLinkList(head, tail);
	_getch();
}
