--救世の美神ノースウェムコ
function c61757117.initial_effect(c)
	c:EnableReviveLimit()
	--set target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetDescription(aux.Stringid(61757117,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c61757117.condition)
	e1:SetTarget(c61757117.target)
	e1:SetOperation(c61757117.operation)
	c:RegisterEffect(e1)
	--indestructable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e2:SetCondition(c61757117.indcon)
	e2:SetValue(1)
	c:RegisterEffect(e2)
end
function c61757117.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_RITUAL
end
function c61757117.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsFaceup() and chkc~=e:GetHandler() end
	if chk==0 then return true end
	local c=e:GetHandler()
	local ct=c:GetMaterialCount()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,ct,c)
end
function c61757117.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc=g:GetFirst()
	local c=e:GetHandler()
	if c:IsFacedown() or not c:IsRelateToEffect(e) then return end
	while tc do
		if tc:IsFaceup() and tc:IsRelateToEffect(e) then c:SetCardTarget(tc) end
		tc=g:GetNext()
	end
end
function c61757117.indcon(e)
	return e:GetHandler():GetCardTargetCount()>0
end
