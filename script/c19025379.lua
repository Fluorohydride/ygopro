--ロード・オブ・ザ・レッド
function c19025379.initial_effect(c)
	c:EnableReviveLimit()
	--destroy monster
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(19025379,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetCondition(c19025379.descon)
	e1:SetTarget(c19025379.destg1)
	e1:SetOperation(c19025379.desop1)
	c:RegisterEffect(e1)
	--destroy spell & trap
	local e2=e1:Clone()
	e2:SetDescription(aux.Stringid(19025379,1))
	e2:SetTarget(c19025379.destg2)
	e2:SetOperation(c19025379.desop2)
	c:RegisterEffect(e2)
end
function c19025379.descon(e,tp,eg,ep,ev,re,r,rp)
	return not re:GetHandler():IsCode(19025379) and not e:GetHandler():IsStatus(STATUS_BATTLE_DESTROYED)
end
function c19025379.destg1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c19025379.desfilter2(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c19025379.destg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c19025379.desfilter2(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c19025379.desfilter2,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c19025379.desfilter2,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c19025379.desop1(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsType(TYPE_MONSTER) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c19025379.desop2(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
