--カース・サイキック
function c16678947.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c16678947.condition)
	e1:SetTarget(c16678947.target)
	e1:SetOperation(c16678947.activate)
	c:RegisterEffect(e1)
end
function c16678947.condition(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=eg:GetFirst()
	local bc=tc:GetBattleTarget()
	return tc:GetPreviousControler()==tp and tc:IsLocation(LOCATION_GRAVE) and tc:IsRace(RACE_PSYCHO)
		and bit.band(tc:GetBattlePosition(),POS_FACEUP)~=0
		and bc:IsRelateToBattle() and bc:IsControler(1-tp) and bc==Duel.GetAttacker()
end
function c16678947.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local tc=eg:GetFirst()
	local bc=Duel.GetAttacker()
	if chk==0 then return bc:IsCanBeEffectTarget(e) and bc:IsDestructable() end
	local lv=tc:GetLevel()
	e:SetLabel(lv)
	Duel.SetTargetCard(bc)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,bc,1,0,0)
	if lv~=0 then
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,lv*300)
	end
end
function c16678947.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		Duel.Damage(1-tp,e:GetLabel()*300,REASON_EFFECT)
	end
end
