package edu2.iastate.cs2280.hw3;

import java.util.AbstractSequentialList;

import java.util.Arrays;
import java.util.Comparator;
import java.util.Iterator;
import java.util.ListIterator;
import java.util.NoSuchElementException;

/** @Aidan Sousley
 * Implementation of the list interface based on linked nodes
 * that store multiple items per node.  Rules for adding and removing
 * elements ensure that each node (except possibly the last one)
 * is at least half full.
 */
public class StoutList<E extends Comparable<? super E>> extends AbstractSequentialList<E>
{
  /**
   * Default number of elements that may be stored in each node.
   */
  private static final int DEFAULT_NODESIZE = 4;
  
  /**
   * Number of elements that can be stored in each node.
   */
  private final int nodeSize;
  
  /**
   * Dummy node for head.  It should be private but set to public here only  
   * for grading purpose.  In practice, you should always make the head of a 
   * linked list a private instance variable.  
   */
  public Node head;
  
  /**
   * Dummy node for tail.
   */
  private Node tail;
  
  /**
   * Number of elements in the list.
   */
  private int size;
  
  /**
   * Constructs an empty list with the default node size.
   */
  public StoutList()
  {
    this(DEFAULT_NODESIZE);
  
  }

  /**
   * Constructs an empty list with the given node size.
   * @param nodeSize number of elements that may be stored in each node, must be 
   *   an even number
   */
  public StoutList(int nodeSize)
  {
    if (nodeSize <= 0 || nodeSize % 2 != 0) throw new IllegalArgumentException();
    
    // dummy nodes
    head = new Node();
    tail = new Node();
    head.next = tail;
    tail.previous = head;
    this.nodeSize = nodeSize;
    
  }
  
  /**
   * Constructor for grading only.  Fully implemented. 
   * @param head
   * @param tail
   * @param nodeSize
   * @param size
   */
  public StoutList(Node head, Node tail, int nodeSize, int size)
  {
	  this.head = head; 
	  this.tail = tail; 
	  this.nodeSize = nodeSize; 
	  this.size = size; 
  }

  @Override
  public int size()
  {
    // TODO Auto-generated method stub
    return size;
  }
  
  public boolean contains(E item) { //Helper method to iterate through data and return if it has the item
	  Node n = head.next;
	  
	  while (n != tail) {
		  for (int i = 0; i < n.count; i++) {
		  if (item == n.data[i]) {
			  return true;
		  }
		  n = n.next;
	  }
	  }
	  return false;
  }
  
  
  @Override
  public boolean add(E item)
  {
	  if(contains(item)) { //Preconditions
		  return false;
	  }
    if (item == null) {
    	throw new NullPointerException();
    }
    
    if (size == 0) {
    	Node n = new Node();
    	head.next = n;
    	n.previous = head;
    	n.next = tail;
    	tail.previous = n;
    	n.addItem(item);
    	
    	
    }
    else {
    	if(tail.previous.count < nodeSize) {
    		tail.previous.addItem(item);
    		
    	}
    	else {
    		Node n = new Node();
    		
    		tail.previous.next = n;
    		n.previous = tail.previous;
    		tail.previous = n;
    		n.next = tail;
    		
    		n.addItem(item);
    	}
    	
    }
    size++;
    int s = size;
    return true;
  }

  @Override
  public void add(int pos, E item)
  {
	  
	  if (item == null) {
	    	throw new NullPointerException();
	    }
	  int s = size;
	  if (pos > size || pos < 0) {
		  throw new IndexOutOfBoundsException();
	  }
	  NodeInfo info = find(pos);
	  int offset = info.offset;
	  Node n = info.node;
	  
	  if (offset == nodeSize) {
		  if(n.next == tail) {
			  Node newN = new Node();
			  newN.previous = n;
			  n.next = newN;
			  newN.next = tail;
			  newN.addItem(item);
			  size++;
			  return;
			  
		  }
		  else {
			  n = n.next;
			  offset = 0;
		  }
	  }
	  
	  
	  if (tail.previous == head) {
		  n = new Node();
		  n.next = tail;
		  tail.previous = n;
		  n.previous = head;
		  head.next = n;
		  
		  n.addItem(item);
		
	  }
	  
	  else if(offset == 0) {
		  
		  if(n.previous.count < size) {
			  n.previous.addItem(item);
			 
		  }
		  else if (tail == n && tail.previous.count == size) {
			  n.next = tail;
			  tail.previous.next = n;
			  n.previous = tail.previous;
			  tail.previous = n;
			  
		  } 
	  }
	   else if (n.count < nodeSize) {
	  		  n.addItem(offset, item);
	  		 
		  }
	  
	   else {
		   int half = nodeSize / 2;
		   Node newN = new Node();
		   newN.previous = n;
		   newN.next = n.next;
		   n.next.previous = newN;
		   n.next = newN;
		   
		   

		   
		   for(int i = half; i < nodeSize; i++) {
			   newN.addItem(n.data[i]);
		   }
		   for(int i = nodeSize; i > half; i--) {
			   n.removeItem(i);
		   }
		   
		   if(offset <= half) {
			   n.addItem(offset, item);

		   }
		   else {
			   newN.addItem(offset - half, item);
		   }
	  }
	  size++;
	  s = size;
	  return;
	  
  }

  @Override
  public E remove(int pos)
  {
	  if (pos > size || pos < 0) {
		  throw new IndexOutOfBoundsException();
	  }
	  
	  NodeInfo info = find(pos);
	  int offset = info.offset;
	  Node n = info.node;
	  
	  if(offset == nodeSize) {
		  n = n.next;
		  offset = 0;
	  }
	  else if(n.data[offset] == null) {
		  int i;
		  while (n != tail) {
			  for (i = offset; i < nodeSize; i++) {
				  if (n.data[i] == null) {
					  continue;
				  }
				  else {
					  offset = i;
				  }
				  break;
			  }
			  if (i != offset) {
				  n = n.next;
				  offset = 0;
				  continue;
			  }
			  break;
		  }
		  if (n.data[offset] == null) {
			  return null;
		  }
	  }
	  
	  if (n.count == 1) {
		  n.next.previous = n.previous;
		  n.previous.next = n.next;
		  n.next = null;
		  n.previous = null;
		  E temp = n.data[0];
		  n.data[0] = null;
		  n = null;
		  size--;
		  return temp;
	  }
	  
	  else if(n.count > nodeSize / 2) {
		  n.removeItem(offset);
		  
	  }
	  else {
		   Node newN = n.next;
		   
		   if (newN.next == null) {
			   n.removeItem(offset);
			   size--;
			   return n.data[offset];
		   }
		   
		   n.removeItem(offset);
		   
		   if (newN.count > nodeSize / 2) {
			   n.addItem(newN.data[0]);
			   newN.removeItem(0);
		   }

		   else {
			  
			   for(int i = 0; i < newN.count; i++) {
				   n.addItem(newN.data[i]);
				   
			   }
			   n.next = newN.next;
			   newN.next.previous = n;
			   newN = null;
			   }
	  }
	  size--;
	  int s = size;
	  return n.data[offset];
  }

  
  /**
   * Sort all elements in the stout list in the NON-DECREASING order. You may do the following. 
   * Traverse the list and copy its elements into an array, deleting every visited node along 
   * the way.  Then, sort the array by calling the insertionSort() method.  (Note that sorting 
   * efficiency is not a concern for this project.)  Finally, copy all elements from the array 
   * back to the stout list, creating new nodes for storage. After sorting, all nodes but 
   * (possibly) the last one must be full of elements.  
   *  
   * Comparator<E> must have been implemented for calling insertionSort().    
   */
  public void sort()
  {
	  Node n = head.next;
	  int count = 0;
	  int j = 0;
	  
	  while (n != tail) {
		  count += n.count;
		  n = n.next;
	  }
	  
	  StoutList.NodeInfo info;
	  E dataSort[] = (E[]) new Comparable[count];
	  n = head.next;
	  
	  while (n != tail) {
		  for (int i = 0; i < n.count; i++) {
			  dataSort[j] = n.data[i];
			  j++;
		  }
		  
		  n = n.next;
		  
		  for (int i = n.previous.count; i > 0; i--) {
			  n.previous.removeItem(i);
		  }
	  }
	  Comparator<E> comp = (a, b) -> a.compareTo(b);
	  insertionSort(dataSort, comp);
	  
  }
  
  /**
   * Sort all elements in the stout list in the NON-INCREASING order. Call the bubbleSort()
   * method.  After sorting, all but (possibly) the last nodes must be filled with elements.  
   *  
   * Comparable<? super E> must be implemented for calling bubbleSort(). 
   */
  public void sortReverse() 
  {
	  Node n = head.next;
	  int count = 0;
	  int j = 0;
	  
	  while (n != tail) {
		  count += n.count;
		  n = n.next;
	  }
	  
	  StoutList.NodeInfo info;
	  E dataSort[] = (E[]) new Comparable[count];
	  n = head.next;
	  
	  while (n != tail) {
		  for (int i = 0; i < n.count; i++) {
			  dataSort[j] = n.data[i];
			  j++;
		  }
		  
		  n = n.next;
		  
		  for (int i = n.previous.count; i > 0; i--) {
			  n.previous.removeItem(i);
		  }
	  }
	
	  bubbleSort(dataSort);
  }
  
  @Override
  public Iterator<E> iterator() 
  { 
		  return new StoutIterator();
  }

  @Override
  public ListIterator<E> listIterator()
  {
    // TODO Auto-generated method stub
    return new StoutListIterator();
  }

  @Override
  public ListIterator<E> listIterator(int index)
  {
    if (index < 0) {
    	throw new IndexOutOfBoundsException();
    }
    return new StoutListIterator(index);
  }
  
  public class StoutIterator implements Iterator<E>{
	  public int cursor;
	  public boolean canRemove = false;
	  public E[] dataList;

	  
	  public void setUpNodes() {
		  Node n = head.next;
		  dataList = (E[]) new Comparable[size];
		  int j = 0;
		  
		  while(n != tail) {
			  for(int i = 0; i < n.count; i++) {
				 dataList[j] = n.data[i];
				 j++;
			  }
			  n = n.next;
		  }
	  }
	@Override
	public boolean hasNext() {
		if (cursor < size) {
			return true;
		}
		else {
			return false;
		}
		
	}

	@Override
	public E next() {
		// TODO Auto-generated method stub
		if (cursor >= size) {
			throw new NoSuchElementException();
		}
		setUpNodes();
		canRemove = true;
		
		E nextElement = dataList[cursor];
		cursor++;
		return nextElement;
	}
	  
  }
  
  /**
   * Returns a string representation of this list showing
   * the internal structure of the nodes.
   */
  public String toStringInternal()
  {
    return toStringInternal(null);
  }

  /**
   * Returns a string representation of this list showing the internal
   * structure of the nodes and the position of the iterator.
   *
   * @param iter
   *            an iterator for this list
   */
  public String toStringInternal(ListIterator<E> iter) 
  {
      int count = 0;
      int position = -1;
      if (iter != null) {
          position = iter.nextIndex();
      }

      StringBuilder sb = new StringBuilder();
      sb.append('[');
      Node current = head.next;
      while (current != tail) {
          sb.append('(');
          E data = current.data[0];
          if (data == null) {
              sb.append("-");
          } else {
              if (position == count) {
                  sb.append("| ");
                  position = -1;
              }
              sb.append(data.toString());
              ++count;
          }

          for (int i = 1; i < nodeSize; ++i) {
             sb.append(", ");
              data = current.data[i];
              if (data == null) {
                  sb.append("-");
              } else {
                  if (position == count) {
                      sb.append("| ");
                      position = -1;
                  }
                  sb.append(data.toString());
                  ++count;

                  // iterator at end
                  if (position == size && count == size) {
                      sb.append(" |");
                      position = -1;
                  }
             }
          }
          sb.append(')');
          current = current.next;
          if (current != tail)
              sb.append(", ");
      }
      sb.append("]");
      return sb.toString();
  }


  /**
   * Node type for this list.  Each node holds a maximum
   * of nodeSize elements in an array.  Empty slots
   * are null.
   */
  private class Node
  {
    /**
     * Array of actual data elements.
     */
    // Unchecked warning unavoidable.
    public E[] data = (E[]) new Comparable[nodeSize];
    
    /**
     * Link to next node.
     */
    public Node next;
    
    /**
     * Link to previous node;
     */
    public Node previous;
    
    /**
     * Index of the next available offset in this node, also 
     * equal to the number of elements in this node.
     */
    public int count;

    /**
     * Adds an item to this node at the first available offset.
     * Precondition: count < nodeSize
     * @param item element to be added
     */
    void addItem(E item)
    {
    	if (item == null) {
    		throw new NullPointerException();
    	}
    	if (count >= nodeSize)
    	{
    		return;
    	}
    	data[count++] = item;
      
      
      //useful for debugging
      //      System.out.println("Added " + item.toString() + " at index " + count + " to node "  + Arrays.toString(data));
    }
  
    /**
     * Adds an item to this node at the indicated offset, shifting
     * elements to the right as necessary.
     * 
     * Precondition: count < nodeSize
     * @param offset array index at which to put the new element
     * @param item element to be added
     */
    void addItem(int offset, E item)
    {
      if (count < nodeSize)
      {
    	  
      
      for (int i = count - 1; i >= offset; --i)
      {
        data[i + 1] = data[i];
      }
      
      ++count;
      data[offset] = item;
      //useful for debugging 
//      System.out.println("Added " + item.toString() + " at index " + offset + " to node: "  + Arrays.toString(data));
    }
      else {
    	  throw new IndexOutOfBoundsException();
      }
    }

    
    /**
     * Deletes an element from this node at the indicated offset, 
     * shifting elements left as necessary.
     * Precondition: 0 <= offset < count
     * @param offset
     */
    void removeItem(int offset)
    {
    	if (offset < 0 || offset > count) {
    		throw new NoSuchElementException();
    	}
      
      for (int i = offset + 1; i < nodeSize; ++i)
      {
        data[i - 1] = data[i];
      }
      data[count - 1] = null;
      --count;
    }    
  }
  
  //Helper class to store which node and what offset in the node
  // a target is at
  
  private class NodeInfo{
	  public Node node;
	  public int offset;
	  public NodeInfo(Node node, int offset) {
		  this.node = node;
		  this.offset = offset;
	  }
  }
  
  //Helper method to find target offset and node
  
  public NodeInfo find(int offset) {
	  int pos = 0;
	  Node node = head.next;
	  NodeInfo info;
	  if(node.next == tail) {					 //precondition so we aren't assigning node to tail
		  info = new NodeInfo(node, offset);
		  node = node.next;
		  return info;
	  }
	  pos = node.count;
	  if(pos > offset) {
		  info = new NodeInfo(node, offset);
		  return info;
	  }
	  
	  else {
	  
	  while (node != tail) {
		  if(pos + node.next.count >= offset) {
			  
			  break;
		  }
		  else {
			  pos += node.count;
			  node = node.next;
		  }
	  }
	  }
	  info = new NodeInfo(node.next, offset - pos);
	  return info;
	  
  }
 
  private class StoutListIterator implements ListIterator<E>
  {
	// constants you possibly use ...   
	  public final int PREV = -1;
	  public final int NEXT = 1;
	  
	  
	// instance variables ... 
	  public E[] data;
	  public int cursor; 
	  public int direction = 0; //Tracks which direction the iterator was last called
	  public boolean canRemove = false; //Prevents remove form being called with next() first
	  
    /**
     * Default constructor 
     */
    public StoutListIterator()
    {
    	cursor = 0;
    	direction = -1;
    	data = (E[]) new Comparable[size];
    	
    	Node n = head.next;
    	int j = 0;
    	
    	while (n != tail) {
    		for( int i = 0; i < n.count; i++) {
    			data[j] = n.data[i];
    			j++;
    		}
    		n = n.next;
    	}
    }

    /**
     * Constructor finds node at a given position.
     * @param pos
     */
    public StoutListIterator(int pos)
    {
    	this.cursor = pos;
    	direction = 0;
 
    	data = (E[]) new Comparable[size];
    	
    	Node n = head.next;
    	int j = 0;
    	
    	while (n != tail) {
    		for( int i = 0; i < n.count; i++) {
    			this.data[j] = n.data[i];
    			j++;
    		}
    		n = n.next;
    	}
    }

    @Override
    public boolean hasNext()
    {
    	// Checks if cursor is smaller than data length
    	
    	if (cursor < data.length) {
    		return true;
    	}
    	
    	return false;
    }

    @Override
    public E next()
    {
    	if (hasNext() == false) {
    		throw new NoSuchElementException(); 
    	}
    	canRemove = true;
		direction = NEXT;
		
		return data[cursor++];
    }

    @Override
    public void remove()
    {
    	
    	int s  = size; // to check size while debugging
    	
    	if (canRemove == true) {
    		if (direction == NEXT) {
    			StoutList.this.remove(cursor - 1);
    			cursor--;

    		}
    		else if( direction == PREV) {
    			StoutList.this.remove(cursor );

    		}
    		Node n = head.next;
    		int j = 0;
    		
    		data = (E[]) new Comparable[size];
    		while (n != tail) {
        		for( int i = 0; i < n.count; i++) {
        			this.data[j] = n.data[i];
        			j++;
        		}
        		n = n.next;
        	}
    		direction = PREV;
    		canRemove = false;
    	}
    	else {
    		throw new IllegalStateException();
    	}
    	s  = size; // to check size while debugging

    }

	@Override
	public boolean hasPrevious() {
		if (cursor > 0) {
			return true;
		}
		return false;
	}

	@Override
	public E previous() {
		if (hasPrevious() == false) {
			throw new NoSuchElementException();
		}
		canRemove = true;
		direction = PREV;
		cursor -= 1;
		
		return data[cursor];
	}

	@Override
	public int nextIndex() {
		// TODO Auto-generated method stub
		return cursor;
	}

	@Override
	public int previousIndex() {
		// TODO Auto-generated method stub
		return cursor - 1;
	}

	@Override
	public void set(E e) {
		// TODO Auto-generated method stub
		if(e == null) { 						// Preconditions
			throw new NullPointerException();
		}
		if(direction == 0) {
			throw new IllegalStateException();
		}
		
		if (direction == NEXT) {
			data[cursor - 1] = e;
			NodeInfo info = find(cursor - 1);
			Node n = info.node;
			n.data[info.offset] = e;
	
		}
		else if (direction == PREV) {
			data[cursor] = e;
			NodeInfo info = find(cursor);
			info.node.data[info.offset] = e;
			
		}
		else {
			throw new IllegalStateException();
		}

		
	}

	@Override
	public void add(E e) {
	
		direction = PREV;
		if(e == null) {
			throw new NullPointerException();
		}
		StoutList.this.add(cursor, e);

		data = (E[]) new Comparable[size];
    	
    	Node n = head.next;
    	int j = 0;
    	
    	while (n != tail) {
    		for( int i = 0; i < n.count; i++) {
    			data[j] = n.data[i];
    			j++;
    		}
    		n = n.next;
    	}
    	cursor++;
    	
	}
    
    // Other methods you may want to add or override that could possibly facilitate 
    // other operations, for instance, addition, access to the previous element, etc.
    // 
    // ...
    // 
  }
  

  /**
   * Sort an array arr[] using the insertion sort algorithm in the NON-DECREASING order. 
   * @param arr   array storing elements from the list 
   * @param comp  comparator used in sorting 
   */
  private void insertionSort(E[] arr, Comparator<? super E> comp)
  {
	 int length = arr.length;
	 NodeInfo info;
	 Node n = new Node();
	 n.next = tail;
	 n.previous = head;
	 head.next = n;
	 tail.previous = n;
	 
	  for (int i = 1; i < length; i++) { //Actual insertion sort
		  E target = arr[i];
		  int h = i - 1;
		  
		  while (h >= 0 && arr[h].compareTo(target) == 1) { 
			  arr[h + 1] = arr[h];
			  h = h - 1;
		  }
		  arr[h + 1] = target;
	  }
	  
	  
	  int arrPos = 0;
	  while (arrPos < length) { //Creates new nodes
		  for(int j = 0; j < nodeSize; j++) {
			  if(arrPos >= length) {
				  break;
			  }
			  if((arr[arrPos] != null)) {
				  n.addItem(arr[arrPos]);
				  arrPos++;
			  }
			  else {
				  break;
			  }
		  }
		 if (arrPos < length) { //Ensures doesn't make an unneeded node
			  Node newN = new Node();
			  newN.previous = n;
			  newN.next = tail;
			  n.next = newN;
			  n = newN;
		 }
	  }
  }
  
  /**
   * Sort arr[] using the bubble sort algorithm in the NON-INCREASING order. For a 
   * description of bubble sort please refer to Section 6.1 in the project description. 
   * You must use the compareTo() method from an implementation of the Comparable 
   * interface by the class E or ? super E. 
   * @param arr  array holding elements from the list
   */
  private void bubbleSort(E[] arr)
  {

	 Node n = head.next;
	  
	  E temp;
	  int i, h;
	  int length = arr.length;
	  boolean swap; 
	  
	  for (i = 0; i < length - 1; i++) { //Actual bubble sort
		  swap = false;
		  for (h = 0; h < length - i - 1; h++) {
			  if ((arr[h+1] == null)) {
				  
			  }
			  if(arr[h + 1].compareTo(arr[h]) == 1) {
				  temp = arr[h];
				  arr[h] = arr[h+1];
				  arr[h+1] = temp;
				  swap = true;
			  }
		  }
		  if(swap == false) {
			  break;
		  }
	  }
	  
	  int arrPos = 0; //Creates new nodes
	  while (arrPos < length) {
		  for(int l = 0; l < nodeSize; l++) {
			  if(arrPos >= length) {
				  break;
			  }
			  if((arr[arrPos] != null)) {
				  n.addItem(arr[arrPos]);
				  arrPos++;
			  }
			  else {
				  break;
			  }
		  }
		 if (arrPos < length) { //Ensures doesn't make an unneeded node
			  Node newN = new Node();
			  newN.previous = n;
			  newN.next = tail;
			  n.next = newN;
			  n = newN;
		 }
	  }
	  
  }
 

}