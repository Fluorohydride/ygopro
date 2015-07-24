--熟練の赤魔術士
function c65338781.initial_effect(c)
	c:EnableCounterPermit(0x3001)
	c:SetCounterLimit(0x3001,3)
	--add counter
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(aux.chainreg)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVED)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c65338781.acop)
	c:RegisterEffect(e2)
	--special summon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(65338781,0))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCost(c65338781.spcost)
	e3:SetTarget(c65338781.sptg)
	e3:SetOperation(c65338781.spop)
	c:RegisterEffect(e3)
	--counter
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(65338781,1))
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_GRAVE)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e4:SetCost(c65338781.ctcost)
	e4:SetTarget(c65338781.cttg)
	e4:SetOperation(c65338781.ctop)
	c:RegisterEffect(e4)
end
function c65338781.acop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL) and e:GetHandler():GetFlagEffect(1)>0 then
		e:GetHandler():AddCounter(0x3001,1)
	end
end
function c65338781.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetCounter(0x3001)==3 and e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c65338781.filter(c,e,tp)
	return c:IsSetCard(0x45) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c65338781.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c65338781.filter,tp,LOCATION_HAND+LOCATION_DECK+LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND+LOCATION_DECK+LOCATION_GRAVE)
end
function c65338781.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c65338781.filter,tp,LOCATION_HAND+LOCATION_DECK+LOCATION_GRAVE,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c65338781.ctcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c65338781.ctfilter(c)
	return c:IsFaceup() and c:IsCanAddCounter(0x3001,1)
end
function c65338781.cttg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(tp) and c65338781.ctfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c65338781.ctfilter,tp,LOCATION_ONFIELD,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(65338781,2))
	Duel.SelectTarget(tp,c65338781.ctfilter,tp,LOCATION_ONFIELD,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_COUNTER,nil,1,0,0x3001)
end
function c65338781.ctop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		tc:AddCounter(0x3001,1)
	end
end
