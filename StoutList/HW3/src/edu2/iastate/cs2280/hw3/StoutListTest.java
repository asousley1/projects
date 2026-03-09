package edu2.iastate.cs2280.hw3;

import static org.junit.jupiter.api.Assertions.*;

import java.util.InputMismatchException;
import java.util.Iterator;
import java.util.ListIterator;
import java.util.NoSuchElementException;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;


class StoutListTest {

	private StoutList<Integer> sl;
	
	@BeforeEach
	void setUp() throws Exception {
		sl = new StoutList<>();
		sl.add(1);
		sl.add(2);
		sl.add(2, 3);
		sl.add(3, 4);
		sl.add(5);
	}
	
	/*  public static void main(String args[]) {
		StoutList<Integer> sl2 = new StoutList<>();
		sl2.add(1);
		sl2.add(2);
		sl2.add(2, 3);
		sl2.add(3, 4);
		sl2.add(5);
		sl2.add(2, 6);
		
		System.out.println(sl2.toStringInternal());
	}*/

	@Test
	void testGeneral() {
		// Constructor
		assertThrows(Exception.class, ()->{
			new StoutList<Integer>(5);
		});
		assertEquals(sl.size(), 5);
		assertEquals("[(1, 2, 3, 4), (5, -, -, -)]", sl.toStringInternal());
		
		// split
		sl.add(2, 6);
		assertEquals(sl.toStringInternal(),"[(1, 2, 6, -), (3, 4, -, -), (5, -, -, -)]");
	
		// full merge
		sl.remove(2);
		assertTrue(sl.toStringInternal().equals("[(1, 2, -, -), (3, 4, -, -), (5, -, -, -)]"));
		assertEquals(sl.size(), 5);
		sl.remove(2); 
		assertTrue(sl.toStringInternal().equals("[(1, 2, -, -), (4, 5, -, -)]"));

		// mini-merge
		sl.add(4, 6);
		assertEquals(sl.size(), 5);
		sl.add(3, 7);
	//	assertTrue(sl.toStringInternal().equals("[(1, 2, -, -), (4, 7, 5, 6)]"));
		assertEquals(sl.toStringInternal(), "[(1, 2, -, -), (4, 7, 5, 6)]");
		sl.remove(0); 
		assertTrue(sl.toStringInternal().equals("[(2, 4, -, -), (7, 5, 6, -)]"));
		assertEquals(sl.size(), 5);
		
		// delete last node
		sl.add(8);
		sl.add(9);
		assertEquals(sl.size(), 7);
	//	assertTrue(sl.toStringInternal().equals("[(2, 4, -, -), (7, 5, 6, 8), (9, -, -, -)]"));
		assertEquals("[(2, 4, -, -), (7, 5, 6, 8), (9, -, -, -)]", sl.toStringInternal());
		sl.remove(6);
		assertEquals(sl.size(), 6);
		assertTrue(sl.toStringInternal().equals("[(2, 4, -, -), (7, 5, 6, 8)]"));
		sl.add(6, 9);
		sl.remove(5);
		assertEquals(sl.size(), 6);
		//assertTrue(sl.toStringInternal().equals("[(2, 4, -, -), (7, 5, 6, -), (9, -, -, -)]"));
		assertEquals("[(2, 4, -, -), (7, 5, 6, -), (9, -, -, -)]", sl.toStringInternal());
		sl.remove(5);
	//	assertTrue(sl.toStringInternal().equals("[(2, 4, -, -), (7, 5, 6, -)]"));
		assertEquals("[(2, 4, -, -), (7, 5, 6, -)]", sl.toStringInternal());
		
		// set

		
		// index out of bounds
		assertThrows(IndexOutOfBoundsException.class, ()->{
			sl.add(-1, 5);
		});
		assertThrows(IndexOutOfBoundsException.class, ()->{
			sl.add(6, 5);
		});
		assertThrows(IndexOutOfBoundsException.class, ()->{
			sl.remove(-1);
		});
		assertThrows(IndexOutOfBoundsException.class, ()->{
			sl.remove(5);
		});
		
		// size
		assertEquals(sl.size(), 5);
		
		// add null
		assertThrows(NullPointerException.class, ()->{
			sl.add(null);
		});
		assertThrows(NullPointerException.class, ()->{
			sl.add(0, null);
		});
		
		System.out.println(sl.toStringInternal());
	}
	
	
	@Test
	void testIterator() {
		assertThrows(IndexOutOfBoundsException.class, ()->{
			sl.listIterator(-1);
		});
		
		Iterator<Integer> iter = sl.iterator();
		// next
		for (int i = 0; i < 5; i++) {
			assertEquals(iter.next(), i + 1);
		}
		
		/* remove
		iter = sl.iterator();
		iter.next();
		iter.remove();
		iter.next();
		iter.remove();
		assertTrue(sl.toStringInternal().equals("[(3, 4, -, -), (5, -, -, -)]"));
		
		// hasNext
		while (iter.hasNext()) {
			iter.next();
			iter.remove();
		}
		
		assertTrue(sl.toStringInternal().equals("[]"));
		
		System.out.println(sl.toStringInternal());*/
	}
	
	
	@Test
	void testListIterator() {
		// Constructor
		assertThrows(IndexOutOfBoundsException.class, ()->{
			sl.listIterator(-1);
		});
		
		ListIterator<Integer> iter = sl.listIterator(1);
		assertTrue(sl.toStringInternal(iter).equals("[(1, | 2, 3, 4), (5, -, -, -)]"));
		
		// next
		iter = sl.listIterator();
		for (int i = 0; i < 5; i++) {
			assertEquals(iter.next(), i + 1);
		}
		
		// previous
		iter = sl.listIterator(5);
		for (int i = 0; i < 5; i++) {
			assertEquals(iter.previous(), 5 - i);
		}
		
		// going out of bounds
		assertThrows(NoSuchElementException.class, ()->{
			ListIterator<Integer> iter2 = sl.listIterator(0);
			iter2.previous();
		});
		assertThrows(NoSuchElementException.class, ()->{
			ListIterator<Integer> iter2 = sl.listIterator(5);
			iter2.next();
		});
		
		// set null
		assertThrows(NullPointerException.class, ()->{
			ListIterator<Integer> iter2 = sl.listIterator(4);
			iter2.next();
			iter2.set(null);
		});
		
		// set with direction NONE
		assertThrows(IllegalStateException.class, ()->{
			ListIterator<Integer> iter2 = sl.listIterator(3);
			iter2.set(7777);

		});
	/*	assertThrows(IllegalStateException.class, ()->{
			ListIterator<Integer> iter2 = sl.listIterator(4);
			iter2.add(99);
			iter2.previous();
			iter2.remove();
			iter2.set(7777);
		});*/
		
		// add null
		assertThrows(NullPointerException.class, ()->{
			ListIterator<Integer> iter2 = sl.listIterator(4);
			iter2.add(null);
		});
		
		// set
		sl.add(2, 6);
		iter = sl.listIterator(0);
		iter.next();
		iter.next();
		iter.next();
		iter.set(30);
		iter.next();
		iter.previous();
		iter.set(40);
		iter.set(41);		
		//assertTrue(sl.toStringInternal().equals("[(1, 2, 30, -), (41, 4, -, -), (5, -, -, -)]"));
		assertEquals(sl.toStringInternal(), "[(1, 2, 30, -), (41, 4, -, -), (5, -, -, -)]");
		assertEquals(sl.size(), 6);
		
		// remove
		iter.remove();
		assertEquals(sl.size(), 5);
		assertEquals(sl.toStringInternal(iter), "[(1, 2, 30, -), (| 4, 5, -, -)]");
		
		// next and previous return value
		assertEquals(iter.previous(), 30);
		assertEquals(iter.next(), 30);
		
		// remove
		iter.previous();
		iter.remove();
		iter.previous();
		iter.remove();
		assertEquals(sl.toStringInternal(iter),"[(1, | 4, 5, -)]");
		
		// add
		iter.next();
		iter.add(30);
		assertEquals(sl.size(), 4);
		iter.next();
		iter.add(40);
		assertEquals(sl.size(), 5);
		iter.add(40);
		assertEquals(sl.size(), 6);
		iter.add(40);
		assertEquals(sl.size(), 7);
		iter.add(40);
		assertEquals(sl.toStringInternal(iter), "[(1, 4, 30, 5), (40, 40, 40, 40 |)]");
		
		// hasNext and hasPrevious
		while (iter.hasNext()) {
			iter.next();
		}
		while (iter.hasPrevious()) {
			iter.previous();
			iter.remove();
		}
		assertEquals(sl.toStringInternal(iter), "[]");
		
		sl.add(7);
		sl.add(6);
		
		// nextIndex and previousIndex
		assertEquals(iter.nextIndex(), 0);
		assertEquals(iter.previousIndex(), -1);
		
		// remove 2 times in a row
		assertThrows(IllegalStateException.class, ()->{
			ListIterator<Integer> iter2 = sl.listIterator();
			iter2.next();
			iter2.remove();
			iter2.remove();
		});
		
		//System.out.println(sl.toStringInternal(iter));
	}
	
	@Test
	void testSort() {
		int[] arr = new int[]{17, 14, 18, 15, 6, 13, 20, 19, 3, 21, 8, 7, 11, 2, 12, 16, 5, 9, 1, 10, 4, 23, 22};
		sl = new StoutList<>(4);
		
		for (int n : arr) {
			sl.add(n);
		}
		
		// create a few gaps
		for (int i = 2; i < 14; i += 4) {
			sl.add(i, 7777);
			sl.remove(i);
			//System.out.println(sl.toStringInternal());
		}
		
		assertTrue(sl.toStringInternal().equals("[(17, 14, -, -), (18, 15, -, -), (6, 13, -, -), (20, 19, -, -), (3, 21, -, -), (8, 7, -, -), (11, 2, 12, 16), (5, 9, 1, 10), (4, 23, 22, -)]"));
		
		sl.sort();
		
		assertEquals(sl.toStringInternal(),("[(1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16), (17, 18, 19, 20), (21, 22, 23, -)]"));
		
		//System.out.println(sl.toStringInternal());
	}
	
	
	@Test
	void testSortReverse() {
		int[] arr = new int[]{17, 14, 18, 15, 6, 13, 20, 19, 3, 21, 8, 7, 11, 2, 12, 16, 5, 9, 1, 10, 4, 23, 22};
		sl = new StoutList<>(6); // 6 for fun
		
		for (int n : arr) {
			sl.add(n);
		}
		
		// create a few gaps
		for (int i = 2; i < 14; i += 4) {
			sl.add(i, 7777);
			sl.remove(i);
			//System.out.println(sl.toStringInternal());
		}
		
		assertTrue(sl.toStringInternal().equals("[(17, 14, 18, -, -, -), (15, 6, 13, -, -, -), (20, 19, 3, -, -, -), (21, 8, 7, -, -, -), (11, 2, 12, 16, 5, 9), (1, 10, 4, 23, 22, -)]"));
		
		sl.sortReverse();
		
		assertTrue(sl.toStringInternal().equals("[(23, 22, 21, 20, 19, 18), (17, 16, 15, 14, 13, 12), (11, 10, 9, 8, 7, 6), (5, 4, 3, 2, 1, -)]"));
		
		//System.out.println(sl.toStringInternal());
	}

}
