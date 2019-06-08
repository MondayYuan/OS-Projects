import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveAction;

public class QuickSortTask extends RecursiveAction{

    private int[] array;
    private int left;
    private int right;

    public QuickSortTask(int[] array, int left, int right) {
        this.array = array;
        this.left = left;
        this.right = right;
    }

    @Override
    protected void compute() {
        int pivot = partition(array, left, right);
        QuickSortTask task1 = null;
        QuickSortTask task2 = null;
        if (pivot - left > 1) {
            task1 = new QuickSortTask(array, left, pivot-1);
            task1.fork();
        }
        if (right - pivot > 1) {
            task2 = new QuickSortTask(array, pivot+1, right);
            task2.fork();
        }
        if (task1 != null && !task1.isDone()) {
            task1.join();
        }
        if (task2 != null && !task2.isDone()) {
            task2.join();
        }
    }

    public static int partition(int[] a, int left, int right) {
        int pivot = a[left];
        while (left < right) {
            while (left < right && a[right] >= pivot) {
                right--;
            }
            swap(a, left, right);
            while (left < right && a[left] <= pivot) {
                left++;
            }
            swap(a, left, right);
        }
        return left;
    }

    public static void swap(int[] a, int i, int j) {
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }

    public static void main(String[] args) {
        int[] a = {4,2,1,4,7,5,3,328,2,7,1,78,89,6,5,4,8,5};
        ForkJoinPool forkJoinPool = new ForkJoinPool();
        QuickSortTask task = new QuickSortTask(a, 0, a.length-1);
        forkJoinPool.submit(task);
        try{
            Thread.sleep(1000);
        }catch(Exception e){
            System.out.println(e);
        }
        
        for (int n : a) {
            System.out.print(n + " ");
        }
        System.out.print("\n");
    }
}