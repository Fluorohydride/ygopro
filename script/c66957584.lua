--インフェルニティガン
function c66957584.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--discard
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(66957584,0))
	e2:SetCategory(CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetTarget(c66957584.distg)
	e2:SetOperation(c66957584.disop)
	c:RegisterEffect(e2)
	--special summon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(66957584,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCondition(c66957584.spcon)
	e3:SetCost(c66957584.spcost)
	e3:SetTarget(c66957584.sptg)
	e3:SetOperation(c66957584.spop)
	c:RegisterEffect(e3)
end
function c66957584.disfilter(c)
	return c:IsSetCard(0xb) and c:IsType(TYPE_MONSTER) and c:IsAbleToGrave()
end
function c66957584.distg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c66957584.disfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,0,0)
end
function c66957584.disop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c66957584.disfilter,tp,LOCATION_HAND,0,1,1,nil)
	if g:GetCount()>0 then Duel.SendtoGrave(g,REASON_EFFECT) end
end
function c66957584.spcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)==0
end
function c66957584.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c66957584.filter(c,e,tp)
	return c:IsSetCard(0xb) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c66957584.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c66957584.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c66957584.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ct>2 then ct=2 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c66957584.filter,tp,LOCATION_GRAVE,0,1,ct,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c66957584.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)>0 then return end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	if sg:GetCount()==0 or ft<sg:GetCount() then return end
	Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
end
