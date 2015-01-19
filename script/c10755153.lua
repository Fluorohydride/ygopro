--ガーディアン・シール
function c10755153.initial_effect(c)
	--sum limit
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetCondition(c10755153.sumlimit)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e3)
	--destroy
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(10755153,0))
	e4:SetCategory(CATEGORY_DESTROY)
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_MZONE)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e4:SetCost(c10755153.descost)
	e4:SetTarget(c10755153.destg)
	e4:SetOperation(c10755153.desop)
	c:RegisterEffect(e4)
end
function c10755153.cfilter(c)
	return c:IsFaceup() and c:IsCode(95638658)
end
function c10755153.sumlimit(e)
	return not Duel.IsExistingMatchingCard(c10755153.cfilter,e:GetHandlerPlayer(),LOCATION_ONFIELD,0,1,nil)
end
function c10755153.costfilter(c,ec)
	return c:IsFaceup() and c:GetEquipTarget()==ec and c:IsAbleToGraveAsCost()
end
function c10755153.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c10755153.costfilter,tp,LOCATION_SZONE,0,1,nil,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c10755153.costfilter,tp,LOCATION_SZONE,0,1,1,nil,e:GetHandler())
	Duel.SendtoGrave(g,REASON_COST)
end
function c10755153.desfilter(c)
	return c:IsDestructable()
end
function c10755153.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c10755153.desfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c10755153.desfilter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c10755153.desfilter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c10755153.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
