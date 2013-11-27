--極星獣タングニョースト
function c14677495.initial_effect(c)
	--special summon(hand)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(14677495,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c14677495.spcon1)
	e1:SetTarget(c14677495.sptg1)
	e1:SetOperation(c14677495.spop1)
	c:RegisterEffect(e1)
	--spsummon2
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(14677495,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e2:SetCode(EVENT_CHANGE_POS)
	e2:SetCountLimit(1)
	e2:SetCondition(c14677495.spcon2)
	e2:SetTarget(c14677495.sptg2)
	e2:SetOperation(c14677495.spop2)
	c:RegisterEffect(e2)
end
function c14677495.cfilter(c,tp)
	return c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_BATTLE) and c:GetPreviousControler()==tp
end
function c14677495.spcon1(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c14677495.cfilter,1,nil,tp)
end
function c14677495.sptg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c14677495.spop1(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c14677495.spcon2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetPosition()==POS_FACEUP_ATTACK and e:GetHandler():GetPreviousPosition()>0x3
end
function c14677495.filter(c,e,tp)
	return c:IsSetCard(0x6042) and c:GetCode()~=14677495 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c14677495.sptg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c14677495.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c14677495.spop2(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c14677495.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
	end
end
