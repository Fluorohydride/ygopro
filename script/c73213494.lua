--Calcab, Malebranche of the Burning Abyss
function c73213494.initial_effect(c)
	--self destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SELF_DESTROY)
	e1:SetCondition(c73213494.sdcon)
	c:RegisterEffect(e1)
	--Special Summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(73213494,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_HAND)
	e2:SetCountLimit(1,73213494)
	e2:SetCondition(c73213494.sscon)
	e2:SetTarget(c73213494.sstg)
	e2:SetOperation(c73213494.ssop)
	c:RegisterEffect(e2)
	--to hand
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(73213494,1))
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DELAY)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetCountLimit(1,73213494)
	e3:SetCondition(c73213494.thcon)
	e3:SetTarget(c73213494.thtg)
	e3:SetOperation(c73213494.thop)
	c:RegisterEffect(e3)
end
function c73213494.sdfilter(c)
	return not c:IsFaceup() or not c:IsSetCard(0xb1)
end
function c73213494.sdcon(e)
	return Duel.IsExistingMatchingCard(c73213494.sdfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c73213494.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c73213494.sscon(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c73213494.filter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c73213494.sstg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c73213494.ssop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.IsExistingMatchingCard(c73213494.filter,tp,LOCATION_ONFIELD,0,1,nil) then return end
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c73213494.thfilter(c)
	return c:IsFacedown() and c:IsAbleToHand()
end
function c73213494.thcon(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsReason(REASON_RETURN)
end
function c73213494.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c73213494.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c73213494.thfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c73213494.thfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c73213494.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFacedown() then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
