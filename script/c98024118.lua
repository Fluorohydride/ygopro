--ホワイトポータン
function c98024118.initial_effect(c)
	--lp damage
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetCondition(c98024118.indcon)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--lp damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(98024118,0))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_BATTLE_DESTROYED)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCondition(c98024118.damcon)
	e2:SetTarget(c98024118.damtg)
	e2:SetOperation(c98024118.damop)
	c:RegisterEffect(e2)
end
function c98024118.indfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_TUNER)
end
function c98024118.indcon(e)
	return Duel.IsExistingMatchingCard(c98024118.indfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c98024118.filter(c,tp)
	return c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_BATTLE) and c:IsType(TYPE_TUNER)
		and c:GetPreviousControler()==tp and c:IsPreviousPosition(POS_FACEUP)
end
function c98024118.damcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c98024118.filter,1,nil,tp)
end
function c98024118.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(500)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c98024118.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
