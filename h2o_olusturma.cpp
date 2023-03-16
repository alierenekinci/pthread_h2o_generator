#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <chrono>
#include <random>

int hidrojen_degeri = 0;
int oksijen_degeri = 0;

std::mutex ekran_koruyucu;
std::mutex koruyucu;

std::binary_semaphore oksijen_semaphore(0);
std::binary_semaphore hidrojen_semaphore(0);

int rastgele_sayi_uret(int min, int max) {
	static std::random_device r;
	static std::default_random_engine e1(r());
	std::uniform_int_distribution<int> duzgun_dagilim(min, max); return duzgun_dagilim(e1);
}

void uyu(int max_sure) {
	std::chrono::milliseconds sure{ rastgele_sayi_uret(0,max_sure) };
	std::this_thread::sleep_for(sure);
}

void oksijen_uretme() {
	koruyucu.lock();

	if (oksijen_degeri >= 1) {
		koruyucu.unlock();
		{
			std::lock_guard<std::mutex> ekran_koruma(ekran_koruyucu);
			std::cout << "1 veya 1 den fazla oksijen var uyuyorum.." << std::endl;
		}
		oksijen_semaphore.acquire();
	}
	else {
		oksijen_degeri += 1;
		{
			std::lock_guard<std::mutex> ekran_koruma(ekran_koruyucu);
			std::cout << "oksijen 1 artti.." << std::endl;
		}
		koruyucu.unlock();

	}
}


void hidrojen_uretme() {

	koruyucu.lock();
	if (hidrojen_degeri >= 2) {


		koruyucu.unlock();
		hidrojen_semaphore.acquire();
		{
			std::lock_guard<std::mutex> ekran_koruma(ekran_koruyucu);
			std::cout << "2 den fazla hidrojen var uyuyorum.. " << std::endl;
		}
	}
	else {
		hidrojen_degeri += 1;
		{
			std::lock_guard<std::mutex> ekran_koruma(ekran_koruyucu);
			std::cout << "hidrojen 1 artti.." << std::endl;
		}
		koruyucu.unlock();
	}

}

void olusturan() {
	while (true)
	{
		koruyucu.lock();
		/*
		{
			std::lock_guard<std::mutex> ekran_koruma(ekran_koruyucu);
			std::cout << "o= " << oksijen_degeri << " " << "h= " << hidrojen_degeri << std::endl;
		}

		*/
		if (hidrojen_degeri < 2 && oksijen_degeri < 1) {
			koruyucu.unlock();

			oksijen_semaphore.release();
			hidrojen_semaphore.release();

		}
		else if (hidrojen_degeri < 2 && oksijen_degeri == 1) {
			koruyucu.unlock();
			hidrojen_semaphore.release();
		}
		else if (oksijen_degeri < 1 && hidrojen_degeri == 2) {
			koruyucu.unlock();
			oksijen_semaphore.release();
		}
		else {
			hidrojen_degeri -= 2;
			oksijen_degeri -= 1;
			koruyucu.unlock();
			oksijen_semaphore.release();
			hidrojen_semaphore.release();
			{
				std::lock_guard<std::mutex> ekran_koruma(ekran_koruyucu);
				std::cout << "Su olustu" << std::endl;
			}
		}


	}
}

void oksijen() {
	while (true)
	{
		oksijen_uretme();
		uyu(125);
	}
}

void hidrojen() {
	while (true)
	{
		hidrojen_uretme();
		uyu(125);
	}
}

int main() {

	std::thread oksijen_uretme_thread(oksijen);
	std::thread hidrojen_uretme_thread(hidrojen);
	std::thread olusturan_thread(olusturan);

	hidrojen_uretme_thread.join();
	oksijen_uretme_thread.join();
	olusturan_thread.join();

	return 0;
}