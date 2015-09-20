--ミステリーサークル
function c24082387.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetLabel(0)
	e1:SetCost(c24082387.cost)
	e1:SetTarget(c24082387.target)
	e1:SetOperation(c24082387.activate)
	c:RegisterEffect(e1)
end
function c24082387.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	return true
end
function c24082387.filter1(c,e,tp,cg,minc)
	return c:IsSetCard(0xc) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and cg:CheckWithSumEqual(Card.GetLevel,c:GetLevel(),minc,99)
end
function c24082387.cgfilter(c)
	return c:GetLevel()>0 and c:IsAbleToGraveAsCost() and bit.band(c:GetOriginalType(),TYPE_MONSTER)~=0
end
function c24082387.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local cg=Duel.GetMatchingGroup(c24082387.cgfilter,tp,LOCATION_MZONE,0,nil)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local minc=-ft+1
	if minc<=0 then minc=1 end
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.IsExistingMatchingCard(c24082387.filter1,tp,LOCATION_DECK,0,1,nil,e,tp,cg,minc)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local rg=Duel.SelectMatchingCard(tp,c24082387.filter1,tp,LOCATION_DECK,0,1,1,nil,e,tp,cg,minc)
	e:SetLabel(rg:GetFirst():GetLevel())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local sg=cg:SelectWithSumEqual(tp,Card.GetLevel,e:GetLabel(),minc,99)
	Duel.SendtoGrave(sg,REASON_COST)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c24082387.filter2(c,e,tp,lv)
	return c:IsSetCard(0xc) and c:GetLevel()==lv and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c24082387.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then Duel.Damage(tp,2000,REASON_EFFECT) return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c24082387.filter2,tp,LOCATION_DECK,0,1,1,nil,e,tp,e:GetLabel())
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	else Duel.Damage(tp,2000,REASON_EFFECT) end
end
