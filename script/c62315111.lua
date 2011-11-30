--エーリアン·ハンター
function c62315111.initial_effect(c)
	--check
	e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCode(EVENT_BATTLE_END)
	e1:SetOperation(c62315111.checkop)
	c:RegisterEffect(e1)
	--chain attack
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(62315111,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCode(EVENT_BATTLE_DESTROYING)
	e2:SetCondition(c62315111.atcon)
	e2:SetOperation(c62315111.atop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c62315111.checkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local t=Duel.GetAttackTarget()
	if t and t~=c and t:GetCounter(0xe)>0 then
		e:SetLabel(1)
	else e:SetLabel(0) end
end
function c62315111.atcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetLabelObject():GetLabel()==1 and e:GetHandler():IsChainAttackable()
end
function c62315111.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChainAttack()
end
