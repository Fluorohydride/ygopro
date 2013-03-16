--燃え上がる大海
function c32542011.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c32542011.condition)
	e1:SetTarget(c32542011.target)
	e1:SetOperation(c32542011.activate)
	c:RegisterEffect(e1)
end
function c32542011.cfilter(c)
	return c:IsFaceup() and c:IsLevelAbove(7) and c:IsAttribute(ATTRIBUTE_WATER+ATTRIBUTE_FIRE)
end
function c32542011.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c32542011.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c32542011.cfilter2(c,att)
	return c:IsFaceup() and c:IsAttribute(att)
end
function c32542011.spfilter(c,tid,e,tp)
	local re=c:GetReasonEffect()
	return c:GetTurnID()==tid and c:IsReason(REASON_COST) and re and re:IsHasType(0x7e0) and re:IsActiveType(TYPE_MONSTER)
		and c:IsAttribute(ATTRIBUTE_WATER) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c32542011.desfilter(c)
	return c:IsDestructable()
end
function c32542011.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return 
		(Duel.IsExistingMatchingCard(c32542011.cfilter2,tp,LOCATION_MZONE,0,1,nil,ATTRIBUTE_FIRE)
		and Duel.IsExistingMatchingCard(c32542011.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil))
		or
		(Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c32542011.cfilter2,tp,LOCATION_MZONE,0,1,nil,ATTRIBUTE_WATER)
		and Duel.IsExistingMatchingCard(c32542011.spfilter,tp,LOCATION_GRAVE,0,1,nil,Duel.GetTurnCount(),e,tp))
	end
	if Duel.IsExistingMatchingCard(c32542011.cfilter2,tp,LOCATION_MZONE,0,1,nil,ATTRIBUTE_FIRE) then
		local g=Duel.GetMatchingGroup(c32542011.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
	if Duel.IsExistingMatchingCard(c32542011.cfilter2,tp,LOCATION_MZONE,0,1,nil,ATTRIBUTE_WATER) then
		local g=Duel.GetMatchingGroup(c32542011.spfilter,tp,LOCATION_GRAVE,0,nil,Duel.GetTurnCount(),e,tp)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,Duel.GetLocationCount(tp,LOCATION_MZONE),0,0)
	end
end
function c32542011.activate(e,tp,eg,ep,ev,re,r,rp)
	local opt=0
	if Duel.IsExistingMatchingCard(c32542011.cfilter2,tp,LOCATION_MZONE,0,1,nil,ATTRIBUTE_WATER) then opt=opt+1 end
	if Duel.IsExistingMatchingCard(c32542011.cfilter2,tp,LOCATION_MZONE,0,1,nil,ATTRIBUTE_FIRE) then opt=opt+2 end
	if opt==1 or opt==3 then
		local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
		if ft>0 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local g=Duel.SelectMatchingCard(tp,c32542011.spfilter,tp,LOCATION_GRAVE,0,1,ft,nil,Duel.GetTurnCount(),e,tp)
			if g:GetCount()>0 then
				Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
				Duel.BreakEffect()
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
				local dg=Duel.SelectMatchingCard(tp,c32542011.desfilter,tp,LOCATION_MZONE,0,1,1,nil)
				Duel.Destroy(dg,REASON_EFFECT)
			end
		end
	end
	if opt==2 or opt==3 then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectMatchingCard(tp,c32542011.desfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
		if g:GetCount()>0 then
			Duel.HintSelection(g)
			Duel.Destroy(g,REASON_EFFECT)
			Duel.BreakEffect()
			Duel.DiscardHand(tp,nil,1,1,REASON_DISCARD+REASON_EFFECT)
		end
	end
end
