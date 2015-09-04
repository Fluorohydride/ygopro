--ハーピィ・チャネラー
function c90238142.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(90238142,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1,90238142)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c90238142.spcost)
	e1:SetTarget(c90238142.sptg)
	e1:SetOperation(c90238142.spop)
	c:RegisterEffect(e1)
	--change name
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetCode(EFFECT_CHANGE_CODE)
	e2:SetRange(LOCATION_MZONE+LOCATION_GRAVE)
	e2:SetValue(76812113)
	c:RegisterEffect(e2)
	--change level
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetCode(EFFECT_CHANGE_LEVEL)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCondition(c90238142.lvcon)
	e3:SetValue(7)
	c:RegisterEffect(e3)
end
function c90238142.cfilter(c)
	return c:IsSetCard(0x64) and c:IsDiscardable()
end
function c90238142.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c90238142.cfilter,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.DiscardHand(tp,c90238142.cfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c90238142.filter(c,e,tp)
	return c:IsSetCard(0x64) and c:GetCode()~=90238142 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c90238142.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c90238142.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c90238142.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c90238142.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
	end
end
function c90238142.lvfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_DRAGON)
end
function c90238142.lvcon(e)
	return Duel.IsExistingMatchingCard(c90238142.lvfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
