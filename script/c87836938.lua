--ヴァイロン·ヴァンガード
function c87836938.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(87836938,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_DESTROY)
	e1:SetCondition(c87836938.drcon)
	e1:SetTarget(c87836938.drtg)
	e1:SetOperation(c87836938.drop)
	c:RegisterEffect(e1)
end
function c87836938.drcon(e,tp,eg,ep,ev,re,r,rp)
	local et=e:GetHandler():GetEquipCount()
	e:SetLabel(et)
	return et>0 and e:GetHandler():IsReason(REASON_EFFECT)
end
function c87836938.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsLocation(LOCATION_GRAVE)
		and Duel.IsPlayerCanDraw(tp,e:GetLabel()) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,e:GetLabel())
end
function c87836938.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
