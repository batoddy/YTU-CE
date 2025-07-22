package java;

public class Main {

	public static void main(String[] args) {
		Car nissan_gtr = new Car("34 AA 12","nissan","GTR R35");
		Person batuhan= new Person("Batuhan","Odçıkın",20);
		
		System.out.println(nissan_gtr.getPlate());
		System.out.println(batuhan.getName());
	}
}
