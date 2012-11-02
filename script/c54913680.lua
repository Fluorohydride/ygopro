--六武ノ書
function c54913680.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c54913680.cost)
	e1:SetTarget(c54913680.target)
	e1:SetOperation(c54913680.activate)
	c:RegisterEffect(e1)
end
function c54913680.costfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d)
end
function c54913680.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c54913680.costfilter,2,nil) end
	local g=Duel.SelectReleaseGroup(tp,c54913680.costfilter,2,2,nil)
	Duel.Release(g,REASON_COST)
end
function c54913680.filter(c,e,tp)
	return c:IsCode(63176202) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c54913680.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-2
		and Duel.IsExistingMatchingCard(c54913680.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c54913680.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c54913680.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
