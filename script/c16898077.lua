--ジャイアント・ボマー・エアレイド
function c16898077.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(16898077,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c16898077.descost)
	e2:SetTarget(c16898077.destg)
	e2:SetOperation(c16898077.desop)
	c:RegisterEffect(e2)
	--destroy & damage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(16898077,1))
	e3:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_SUMMON_SUCCESS)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e3:SetCondition(c16898077.damcon)
	e3:SetTarget(c16898077.damtg)
	e3:SetOperation(c16898077.damop)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(16898077,2))
	e5:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e5:SetCode(EVENT_MSET)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e5:SetCondition(c16898077.damcon2)
	e5:SetTarget(c16898077.damtg2)
	e5:SetOperation(c16898077.damop2)
	c:RegisterEffect(e5)
	local e6=e5:Clone()
	e6:SetCode(EVENT_SSET)
	c:RegisterEffect(e6)
	local e7=Effect.CreateEffect(c)
	e7:SetDescription(aux.Stringid(16898077,2))
	e7:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e7:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e7:SetCode(EVENT_CHANGE_POS)
	e7:SetRange(LOCATION_MZONE)
	e7:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e7:SetCondition(c16898077.damcon3)
	e7:SetTarget(c16898077.damtg3)
	e7:SetOperation(c16898077.damop3)
	c:RegisterEffect(e7)
end
function c16898077.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToGraveAsCost,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,Card.IsAbleToGraveAsCost,tp,LOCATION_HAND,0,1,1,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c16898077.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(1-tp) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c16898077.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c16898077.damcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c16898077.dfilter(c,e,sp)
	return c:GetSummonPlayer()==sp and c:IsDestructable() and (not e or c:IsRelateToEffect(e))
end
function c16898077.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c16898077.dfilter,1,nil,nil,1-tp) end
	local g=eg:Filter(c16898077.dfilter,nil,nil,1-tp)
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,800)
end
function c16898077.damop(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c16898077.dfilter,nil,e,1-tp)
	if e:GetHandler():IsRelateToEffect(e) and g:GetCount()~=0 and Duel.Destroy(g,REASON_EFFECT)~=0 then
		Duel.Damage(1-tp,800,REASON_EFFECT)
	end
end
function c16898077.damcon2(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and rp~=tp
end
function c16898077.damtg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,eg:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,800)
end
function c16898077.damop2(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if e:GetHandler():IsRelateToEffect(e) and tc:IsRelateToEffect(e) and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		Duel.Damage(1-tp,800,REASON_EFFECT)
	end
end
function c16898077.damcon3(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and rp~=tp
end
function c16898077.sfilter(c,e)
	return c:IsFacedown() and c:IsDestructable() and (not e or c:IsRelateToEffect(e))
end
function c16898077.damtg3(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c16898077.sfilter,1,nil) end
	local g=eg:Filter(c16898077.sfilter,nil)
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,800)
end
function c16898077.damop3(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c16898077.sfilter,nil,e)
	if e:GetHandler():IsRelateToEffect(e) and g:GetCount()~=0 and Duel.Destroy(g,REASON_EFFECT)~=0 then
		Duel.Damage(1-tp,800,REASON_EFFECT)
	end
end
