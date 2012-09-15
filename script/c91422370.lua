--螺旋式発条
function c91422370.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCost(c91422370.cost)
	e1:SetTarget(c91422370.target)
	e1:SetOperation(c91422370.activate)
	c:RegisterEffect(e1)
end
function c91422370.costfilter(c)
	return c:IsSetCard(0x58) and c:IsAttackAbove(1500)
end
function c91422370.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c91422370.costfilter,1,nil) end
	local sg=Duel.SelectReleaseGroup(tp,c91422370.costfilter,1,1,nil)
	Duel.Release(sg,REASON_COST)
end
function c91422370.filter(c,e,tp)
	return c:IsSetCard(0x58) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c91422370.filter2(c,atk,e,tp)
	return c:IsSetCard(0x58) and c:GetAttack()==atk and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c91422370.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c91422370.filter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c91422370.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c91422370.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
		local atk=g:GetFirst():GetAttack()
		local sg=Duel.GetMatchingGroup(c91422370.filter2,tp,LOCATION_DECK,0,nil,atk,e,tp)
		if sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(91422370,0)) then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local dg=sg:Select(tp,1,1,nil)
			Duel.SpecialSummon(dg,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
