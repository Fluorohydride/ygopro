--サイコ・ソウル
function c33323657.initial_effect(c)
	--recover
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_RECOVER)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c33323657.reccost)
	e1:SetTarget(c33323657.rectg)
	e1:SetOperation(c33323657.recop)
	c:RegisterEffect(e1)
end
function c33323657.filter(c)
	return c:GetLevel()>0 and c:IsRace(RACE_PSYCHO)
end
function c33323657.reccost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c33323657.filter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c33323657.filter,1,1,nil)
	e:SetLabel(g:GetFirst():GetLevel())
	Duel.Release(g,REASON_COST)
end
function c33323657.rectg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetLabel()~=0 end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(e:GetLabel()*300)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,e:GetLabel()*300)
	e:SetLabel(0)
end
function c33323657.recop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end
