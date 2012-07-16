--紫炎の道場
function c47436247.initial_effect(c)
	c:EnableCounterPermit(0x3003)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--add counter
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetOperation(c47436247.ctop)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e3)
	--special summon
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e4:SetDescription(aux.Stringid(47436247,0))
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCost(c47436247.spcost)
	e4:SetTarget(c47436247.sptg)
	e4:SetOperation(c47436247.spop)
	c:RegisterEffect(e4)
end
function c47436247.ctfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d)
end
function c47436247.ctop(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c47436247.ctfilter,1,nil) then
		e:GetHandler():AddCounter(0x3003,1)
	end
end
function c47436247.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	local ct=e:GetHandler():GetCounter(0x3003)
	e:SetLabel(ct)
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c47436247.filter(c,ct,e,tp)
	return c:IsLevelBelow(ct) and (c:IsSetCard(0x3d) or c:IsSetCard(0x20)) 
		and c:IsType(TYPE_EFFECT) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c47436247.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c47436247.filter,tp,LOCATION_DECK,0,1,nil,e:GetHandler():GetCounter(0x3003),e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c47436247.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local ct=e:GetLabel()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c47436247.filter,tp,LOCATION_DECK,0,1,1,nil,ct,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
