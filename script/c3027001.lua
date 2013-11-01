--偽物のわな
function c3027001.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c3027001.condition)
	e1:SetTarget(c3027001.target)
	e1:SetOperation(c3027001.activate)
	c:RegisterEffect(e1)
end
function c3027001.cfilter(c,tp)
	return c:IsType(TYPE_TRAP) and c:IsControler(tp)
end
function c3027001.condition(e,tp,eg,ep,ev,re,r,rp)
	if rp==tp then return false end
	local ex,tg,tc=Duel.GetOperationInfo(ev,CATEGORY_DESTROY)
	return ex and tg~=nil and tg:GetCount()==tc and tg:IsExists(c3027001.cfilter,1,e:GetHandler(),tp)
end
function c3027001.cffilter(c,tp)
	return c:IsFacedown() and c:IsControler(tp)
end
function c3027001.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local ex,tg,tc=Duel.GetOperationInfo(ev,CATEGORY_DESTROY)
	local fg=tg:Filter(c3027001.cffilter,nil,tp)
	Duel.ConfirmCards(1-tp,fg)
	local reg=tg:Filter(c3027001.cfilter,e:GetHandler(),tp)
	local tc=reg:GetFirst()
	while tc do
		tc:RegisterFlagEffect(3027001,RESET_EVENT+0x1fe0000+RESET_CHAIN,0,1)
		tc=reg:GetNext()
	end
end
function c3027001.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DESTROY_REPLACE)
	e1:SetRange(LOCATION_SZONE)
	e1:SetTarget(c3027001.reptg)
	e1:SetValue(c3027001.repvalue)
	e1:SetOperation(c3027001.repop)
	e1:SetLabelObject(re)
	e1:SetReset(RESET_EVENT+0x1fe0000)
	c:RegisterEffect(e1)
end
function c3027001.repfilter(c)
	return c:GetFlagEffect(3027001)~=0
end
function c3027001.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return re==e:GetLabelObject() and eg:IsExists(c3027001.repfilter,1,nil) end
	return true
end
function c3027001.repvalue(e,c)
	return c:GetFlagEffect(3027001)~=0
end
function c3027001.repop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT+REASON_REPLACE)
end
