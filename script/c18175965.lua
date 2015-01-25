--ガーディアン・デスサイス
function c18175965.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--summon limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_SUMMON)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_CANNOT_MSET)
	c:RegisterEffect(e3)
	--special summon
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(18175965,0))
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e4:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e4:SetCode(EVENT_TO_GRAVE)
	e4:SetRange(LOCATION_HAND)
	e4:SetCondition(c18175965.spcon)
	e4:SetTarget(c18175965.sptg)
	e4:SetOperation(c18175965.spop)
	c:RegisterEffect(e4)
	--equip
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(18175965,1))
	e5:SetCategory(CATEGORY_EQUIP)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e5:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e5:SetCode(EVENT_SPSUMMON_SUCCESS)
	e5:SetTarget(c18175965.eqtg)
	e5:SetOperation(c18175965.eqop)
	c:RegisterEffect(e5)
	--disable summon
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_FIELD)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCode(EFFECT_CANNOT_SUMMON)
	e6:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e6:SetTargetRange(1,0)
	c:RegisterEffect(e6)
	local e7=e6:Clone()
	e7:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	c:RegisterEffect(e7)
	--special summon 2
	local e8=Effect.CreateEffect(c)
	e8:SetDescription(aux.Stringid(18175965,2))
	e8:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e8:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e8:SetCode(EVENT_TO_GRAVE)
	e8:SetCondition(c18175965.spcon2)
	e8:SetTarget(c18175965.sptg2)
	e8:SetOperation(c18175965.spop2)
	c:RegisterEffect(e8)
end
function c18175965.cfilter(c,tp)
	return c:IsFaceup() and c:IsControler(tp) and c:GetPreviousControler()==tp
		and c:IsReason(REASON_DESTROY) and c:IsCode(34022290)
end
function c18175965.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c18175965.cfilter,1,nil,tp)
end
function c18175965.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,true,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c18175965.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,true,false,POS_FACEUP)>0 then
		c:CompleteProcedure()
	end
end
function c18175965.filter(c,ec)
	return c:IsCode(81954378) and c:CheckEquipTarget(ec)
end
function c18175965.eqtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingMatchingCard(c18175965.filter,tp,LOCATION_DECK,0,1,nil,e:GetHandler()) end
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,nil,1,tp,LOCATION_DECK)
end
function c18175965.eqop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0 or c:IsFacedown() or not c:IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(18175965,3))
	local g=Duel.SelectMatchingCard(tp,c18175965.filter,tp,LOCATION_DECK,0,1,1,nil,c)
	if g:GetCount()>0 then
		Duel.Equip(tp,g:GetFirst(),c)
	end
end
function c18175965.spcon2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD)
end
function c18175965.sptg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,0,0,tp,1)
end
function c18175965.spop2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if Duel.DiscardHand(tp,nil,1,1,REASON_EFFECT)==0 then return end
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,true,true,POS_FACEUP)>0 then
		c:CompleteProcedure()
	end
end
