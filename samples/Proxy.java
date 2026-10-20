import com.veryant.joe.Invocation;
import com.veryant.joe.JOEException;

public class Proxy {
   private final Object object;
   public Proxy (Object obj) {
      this.object = obj;
   }
   public Object doesNotUnderstand (Invocation msg)  throws JOEException {
      System.out.println ("@@@>" + msg.selector());
      return msg.sendTo(object);
   }
}
