import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveAction;

public class MergeSortTask extends RecursiveAction{
    private int[] array;
    private int left;
    private int right;

    public static void main(String[] args){
        int[] a = {4,2,1,4,7,5,3,8,2,7,1,78,89,6,5,4,8,5};
        ForkJoinPool forkJoinPool = new ForkJoinPool();
        MergeSortTask task = new MergeSortTask(a, 0, a.length-1);
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

    public MergeSortTask(int[] array, int left, int right){
        this.array = array;
        this.left = left;
        this.right = right;
    }

    @Override
    protected void compute(){
        if(right - left > 0){
            int mid = (left + right) / 2;

            // System.out.print("split:");
            // for(int i=left; i<=right; ++i){
            //     System.out.print(array[i] + " ");
            // }
            // System.out.print(" ==> ");
            // for(int i=left; i<=mid; ++i){
            //     System.out.print(array[i] + " ");
            // }
            // System.out.print(" || ");
            // for(int i=mid+1; i<=right; ++i){
            //     System.out.print(array[i] + " ");
            // }
            // System.out.println("");

            
            MergeSortTask task1 = new MergeSortTask(array, left, mid);
            MergeSortTask task2 = new MergeSortTask(array, mid+1, right);
            invokeAll(task1, task2);
            task1.join();
            task2.join();
            merge(array, left, mid, right);
        }
    }

    public static void merge(int a[], int left, int mid, int right){
        int len = right - left + 1;
        int temp[] = new int[len];
        int i = left;
        int j = mid + 1;
        int k = 0;
        while(i<=mid && j<=right){
            if(a[i] <= a[j]){
                temp[k++] = a[i++];
            }else{
                temp[k++] = a[j++];
            }
        }
        while(i <= mid){
            temp[k++] = a[i++];
        }

        while(j <= right){
            temp[k++] = a[j++];
        }

        for(int s=0; s<temp.length; s++){
            a[left++] = temp[s];
        }
    }
}