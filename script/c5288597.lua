--トランスターン
function c5288597.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetLabel(0)
	e1:SetCost(c5288597.cost)
	e1:SetTarget(c5288597.target)
	e1:SetOperation(c5288597.activate)
	c:RegisterEffect(e1)
end
function c5288597.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	return true
end
function c5288597.cfilter(c,e,tp)
	local lv=c:GetLevel()
	return lv>0 and c:IsAbleToGraveAsCost()
		and Duel.IsExistingMatchingCard(c5288597.spfilter,tp,LOCATION_DECK,0,1,nil,lv+1,c:GetRace(),c:GetAttribute(),e,tp)
end
function c5288597.spfilter(c,lv,rc,att,e,tp)
	return c:GetLevel()==lv and c:IsRace(rc) and c:IsAttribute(att) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c5288597.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
			and Duel.IsExistingMatchingCard(c5288597.cfilter,tp,LOCATION_MZONE,0,1,nil,e,tp)
	end
	local g=Duel.SelectMatchingCard(tp,c5288597.cfilter,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
	local tc=g:GetFirst()
	c5288597[0]=tc:GetLevel()
	c5288597[1]=tc:GetRace()
	c5288597[2]=tc:GetAttribute()
	Duel.SendtoGrave(tc,REASON_COST)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c5288597.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c5288597.spfilter,tp,LOCATION_DECK,0,1,1,nil,c5288597[0]+1,c5288597[1],c5288597[2],e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
