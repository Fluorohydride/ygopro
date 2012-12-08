--風林火山
function c1781310.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_HANDES+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0+TIMING_TOHAND)
	e1:SetCondition(c1781310.condition)
	e1:SetTarget(c1781310.target)
	e1:SetOperation(c1781310.activate)
	c:RegisterEffect(e1)
end
function c1781310.cfilter(c,att)
	return c:IsFaceup() and c:IsAttribute(att)
end
function c1781310.dfilter1(c)
	return c:IsDestructable()
end
function c1781310.dfilter2(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c1781310.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c1781310.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_WIND)
		and Duel.IsExistingMatchingCard(c1781310.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_WATER)
		and Duel.IsExistingMatchingCard(c1781310.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_FIRE)
		and Duel.IsExistingMatchingCard(c1781310.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_EARTH)
end
function c1781310.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c1781310.dfilter1,tp,0,LOCATION_MZONE,1,nil)
		or Duel.IsExistingMatchingCard(c1781310.dfilter2,tp,0,LOCATION_ONFIELD,1,nil)
		or Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>=2
		or Duel.IsPlayerCanDraw(tp,2) end
end
function c1781310.activate(e,tp,eg,ep,ev,re,r,rp)
	local off=1
	local ops={}
	local opval={}
	if Duel.IsExistingMatchingCard(c1781310.dfilter1,tp,0,LOCATION_MZONE,1,nil) then
		ops[off]=aux.Stringid(1781310,0)
		opval[off-1]=1
		off=off+1
	end
	if Duel.IsExistingMatchingCard(c1781310.dfilter2,tp,0,LOCATION_ONFIELD,1,nil) then
		ops[off]=aux.Stringid(1781310,1)
		opval[off-1]=2
		off=off+1
	end
	if Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>=2 then
		ops[off]=aux.Stringid(1781310,2)
		opval[off-1]=3
		off=off+1
	end
	if Duel.IsPlayerCanDraw(tp,2) then
		ops[off]=aux.Stringid(1781310,3)
		opval[off-1]=4
		off=off+1
	end
	if off==1 then return end
	local op=Duel.SelectOption(tp,table.unpack(ops))
	if opval[op]==1 then
		local g=Duel.GetMatchingGroup(c1781310.dfilter1,tp,0,LOCATION_MZONE,nil)
		Duel.Destroy(g,REASON_EFFECT)
	elseif opval[op]==2 then
		local g=Duel.GetMatchingGroup(c1781310.dfilter2,tp,0,LOCATION_ONFIELD,nil)
		Duel.Destroy(g,REASON_EFFECT)
	elseif opval[op]==3 then
		local g=Duel.GetFieldGroup(tp,0,LOCATION_HAND):RandomSelect(1-tp,2)
		Duel.SendtoGrave(g,REASON_EFFECT+REASON_DISCARD)
	elseif opval[op]==4 then
		Duel.Draw(tp,2,REASON_EFFECT)
	end
end
