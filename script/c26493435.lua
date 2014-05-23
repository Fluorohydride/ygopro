--希望郷－オノマトピア－
function c26493435.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--add counter
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_DELAY)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c26493435.ctcon)
	e2:SetOperation(c26493435.ctop)
	c:RegisterEffect(e2)
	--atk/def
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_UPDATE_ATTACK)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetValue(c26493435.val)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e4)
	--special summon
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(26493435,0))
	e5:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e5:SetType(EFFECT_TYPE_IGNITION)
	e5:SetRange(LOCATION_SZONE)
	e5:SetCountLimit(1)
	e5:SetCost(c26493435.spcost)
	e5:SetTarget(c26493435.sptg)
	e5:SetOperation(c26493435.spop)
	c:RegisterEffect(e5)
end
function c26493435.ctfilter(c,tp)
	return c:IsFaceup() and c:IsSetCard(0x7f) and c:IsControler(tp)
end
function c26493435.ctcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c26493435.ctfilter,1,nil,tp)
end
function c26493435.ctop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():AddCounter(0x30,1)
end
function c26493435.val(e,c)
	return e:GetHandler():GetCounter(0x30)*200
end
function c26493435.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsCanRemoveCounter(tp,0x30,2,REASON_COST) end
	e:GetHandler():RemoveCounter(tp,0x30,2,REASON_COST)
end
function c26493435.filter(c,e,tp)
	return (c:IsSetCard(0x8f) or c:IsSetCard(0x54) or c:IsSetCard(0x59) or c:IsSetCard(0x82))
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c26493435.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c26493435.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c26493435.spop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c26493435.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
