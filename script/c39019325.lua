--玉砕指令
function c39019325.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetTarget(c39019325.target)
	e1:SetOperation(c39019325.activate)
	c:RegisterEffect(e1)
end
function c39019325.rfilter(c,e)
	local tpe=c:GetType()
	return bit.band(tpe,TYPE_NORMAL)~=0 and bit.band(tpe,TYPE_TOKEN)==0
		and c:IsFaceup() and c:IsLevelBelow(2) and c:IsReleasable() and c:IsReleasableByEffect() and not c:IsImmuneToEffect(e)
end
function c39019325.dfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c39019325.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c39019325.rfilter(chkc,e) end
	if chk==0 then return Duel.IsExistingTarget(c39019325.rfilter,tp,LOCATION_MZONE,0,1,nil,e)
		and Duel.IsExistingMatchingCard(c39019325.dfilter,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local rg=Duel.SelectTarget(tp,c39019325.rfilter,tp,LOCATION_MZONE,0,1,1,nil,e)
	local g=Duel.GetMatchingGroup(c39019325.dfilter,tp,0,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c39019325.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) and Duel.Release(tc,REASON_EFFECT)>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local dg=Duel.SelectMatchingCard(tp,c39019325.dfilter,tp,0,LOCATION_ONFIELD,1,2,nil)
		if dg:GetCount()>0 then
			Duel.Destroy(dg,REASON_EFFECT)
		end
	end
end
