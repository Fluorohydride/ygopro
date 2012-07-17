--天下統一
function c95352218.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_STANDBY_PHASE)
	c:RegisterEffect(e1)
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(95352218,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetOperation(c95352218.adjustop)
	c:RegisterEffect(e2)
end
function c95352218.checklv(g)
	local tc=g:GetFirst()
	local lv=tc:GetLevel()
	tc=g:GetNext()
	while tc do
		if tc:GetLevel()~=lv then return false end
		tc=g:GetNext()
	end
	return true
end
function c95352218.filter1(c)
	return c:IsFaceup() and c:IsLevelAbove(1)
end
function c95352218.filter2(c,lv)
	return c:IsFaceup() and c:GetLevel()~=lv
end
function c95352218.adjustop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local turnp=Duel.GetTurnPlayer()
	local g=Duel.GetMatchingGroup(c95352218.filter1,turnp,LOCATION_MZONE,0,nil)
	if g:GetCount()<2 then return end
	if c95352218.checklv(g) then return end
	Duel.Hint(HINT_SELECTMSG,turnp,aux.Stringid(95352218,1))
	local clv=g:Select(turnp,1,1,nil):GetFirst():GetLevel()
	local dg=g:Filter(c95352218.filter2,nil,clv)
	Duel.SendtoGrave(dg,REASON_EFFECT)
end
