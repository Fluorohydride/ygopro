--アロマージ－ジャスミン
function c96789758.initial_effect(c)
	--extra summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_EXTRA_SUMMON_COUNT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_HAND+LOCATION_MZONE,0)
	e1:SetCondition(c96789758.excon)
	e1:SetTarget(c96789758.extg)
	c:RegisterEffect(e1)
	--draw
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_RECOVER)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCountLimit(1)
	e2:SetCondition(c96789758.drcon)
	e2:SetTarget(c96789758.drtg)
	e2:SetOperation(c96789758.drop)
	c:RegisterEffect(e2)
end
function c96789758.excon(e)
	local tp=e:GetHandlerPlayer()
	return Duel.GetLP(tp)>Duel.GetLP(1-tp)
end
function c96789758.extg(e,c)
	return c:IsRace(RACE_PLANT) and not c:IsCode(96789758)
end
function c96789758.drcon(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp
end
function c96789758.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return ep==tp and e:GetHandler():IsRelateToEffect(e) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c96789758.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TRIGGERING_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
