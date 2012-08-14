--ブラックポータン
function c76218643.initial_effect(c)
	--lp damage
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetCondition(c76218643.indcon)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--lp recover
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(76218643,0))
	e2:SetCategory(CATEGORY_RECOVER)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCondition(c76218643.reccon)
	e2:SetTarget(c76218643.rectg)
	e2:SetOperation(c76218643.recop)
	c:RegisterEffect(e2)
end
function c76218643.indfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_TUNER)
end
function c76218643.indcon(e)
	return Duel.IsExistingMatchingCard(c76218643.indfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c76218643.filter(c,tp)
	return c:IsPreviousPosition(POS_FACEUP) and c:IsType(TYPE_TUNER) 
end
function c76218643.reccon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c76218643.filter,1,nil,tp)
end
function c76218643.rectg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(800)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,800)
end
function c76218643.recop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end
